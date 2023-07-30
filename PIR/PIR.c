
#include <linux/interrupt.h>
#include <linux/module.h> // To Create Modules
#include <linux/init.h> // To init Modules 
#include <linux/moduleparam.h>   // To Create Paramaters to Pass it to Module 
#include <linux/fs.h> //fs -> <file structure> To Create Major Number and Minor Number
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define PIR_GPIO 17
#define LED_PIN_NUM 23
#define SIZE 3



struct class * my_class;
struct device * my_device;
dev_t device_number ;
struct cdev st_characterDevice;

int state = 0;
static unsigned char Data[SIZE] = "";



// GPIO number for the PIR sensor (change this according to your setup)

// IRQ handler function
static irqreturn_t pir_irq_handler(int irq, void *dev_id)
{
    // Read the state of the GPIO pin (HIGH = motion detected, LOW = no motion)
    state = gpio_get_value(PIR_GPIO);
    printk(KERN_INFO "PIR sensor: Motion %s\n", state ? "detected" : "not detected");
    return IRQ_HANDLED;
}
ssize_t Driver_Read (struct file *file, char __user * user_buffer, size_t count, loff_t *offset)
{
    int remaining_bytes;
    char *print_buffer;
    char tmp[3]="";
    remaining_bytes = SIZE - *offset;
    if (remaining_bytes <= 0)
        return 0;  // End of file reached.
    
    if (count > remaining_bytes)
        count = remaining_bytes;

    tmp[0] = state + '0';
    tmp[1] = '\n';


    if (copy_to_user(user_buffer, &tmp[*offset], count))
        return -1;  // Error copying data to user space.
    
    *offset += count; 

    print_buffer = kmalloc(count + 1, GFP_KERNEL);
    if (print_buffer) {
        if (copy_from_user(print_buffer, user_buffer, count)) {
            kfree(print_buffer);
            return -1;
        }
    }


    print_buffer[count] = '\0';  // Null-terminate the string.
    
    printk(KERN_INFO "Data Read: %s\n", print_buffer);
    kfree(print_buffer);

    return (ssize_t)count;

}
ssize_t Driver_Write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset)
{
    int remaining_bytes;
    remaining_bytes = SIZE - *offset;
    if (remaining_bytes <= 0)
        return 0;  // End of file reached.
    if (count > remaining_bytes)
        count = remaining_bytes;  // Don't write more than what's available.


    if (copy_from_user(&Data[*offset], user_buffer, count))
        return -1;  // Error copying data from user space.
    *offset += count;  // Update the file position.

    switch(user_buffer[0])
    {
        case '0':
            gpio_set_value(LED_PIN_NUM , 0);
            //mdelay(500);
            break;
        case '1':
            gpio_set_value(LED_PIN_NUM , 1);
            //mdelay(500);
            break;
    }

    printk("Data was Written  : %c\n" , user_buffer[0]);
    return (ssize_t)count;

}

struct file_operations fops =
{
    .owner=THIS_MODULE,
    .read=Driver_Read,
    .write=Driver_Write
};


static int __init pir_sensor_init(void)
{
    int irq;
    int result;


    result = alloc_chrdev_region(&device_number , 0 , 1 ,"PIR_SENSOR");

    if(0 != result)
    {
        printk("Couldn't register device number !! .. \n");
        return -1;
    }
    
    // Define Device as Character Device 
    cdev_init(&st_characterDevice ,&fops);
    result = cdev_add(&st_characterDevice , device_number,1);
    if(result !=0)
    {
        printk("Registering of Device Failed \n");
        goto CHARACTER_ERROR;
       
    }

    /*  Creating Driver Class dev file */
    if((my_class = class_create(THIS_MODULE , "PIR_CLASS")) == NULL)
    {
        printk("Device Can't be created !\n");
        goto CLASS_ERROR; 
    }
    my_device = device_create(my_class , NULL , device_number , NULL , "PIR_DEV");
    if(my_device == NULL)
    {
        printk("Device class Can't be created \n");
        goto DEVICE_ERROR;
    }

    // Request and configure the GPIO pin
    result = gpio_request(PIR_GPIO, "pir_sensor");
    if (result < 0)
    {
        printk(KERN_ERR "Failed to request GPIO %d\n", PIR_GPIO);
        return result;
    }

    // Set the GPIO pin as input
    result = gpio_direction_input(PIR_GPIO);
    if (result < 0)
    {
        printk(KERN_ERR "Failed to set GPIO %d as input\n", PIR_GPIO);
        gpio_free(PIR_GPIO);
        return result;
    }
    result = gpio_request(LED_PIN_NUM, "GPIO2");
    if (result < 0)
    {
        printk(KERN_ERR "Failed to request GPIO %d\n", LED_PIN_NUM);
        return result;
    }

    // Set the GPIO pin as input
    result = gpio_direction_output(LED_PIN_NUM ,0);
    if (result < 0)
    {
        printk(KERN_ERR "Failed to set GPIO %d as output\n", LED_PIN_NUM);
        gpio_free(LED_PIN_NUM);
        return result;
    }

    // Request an interrupt for the GPIO pin
    irq = gpio_to_irq(PIR_GPIO);
    result = request_irq(irq, pir_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "pir_sensor", NULL);
    if (result < 0)
    {
        printk(KERN_ERR "Failed to request IRQ for GPIO %d\n", PIR_GPIO);
        gpio_free(PIR_GPIO);
        return result;
    }

    printk(KERN_INFO "PIR sensor module initialized\n");
    return 0;

    DEVICE_ERROR:
        class_destroy(my_class);
    CLASS_ERROR:
        cdev_del(&st_characterDevice);    
    CHARACTER_ERROR:
        unregister_chrdev_region(device_number , 1);
        return -1;
}

static void __exit pir_sensor_exit(void)
{
    int irq = gpio_to_irq(PIR_GPIO);

    // Free the IRQ
    free_irq(irq, NULL);

    // Free the GPIO
    gpio_free(PIR_GPIO);
    gpio_free(LED_PIN_NUM);

    cdev_del(&st_characterDevice);
    device_destroy(my_class,device_number);
    class_destroy(my_class);
    unregister_chrdev_region(device_number , 1);

    printk(KERN_INFO "PIR sensor module removed\n");
}

module_init(pir_sensor_init);
module_exit(pir_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sami Selim");
MODULE_DESCRIPTION("PIR sensor kernel module");
