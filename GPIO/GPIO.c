#include <linux/module.h> // To Create Modules
#include <linux/init.h> // To init Modules 
#include <linux/moduleparam.h>   // To Create Paramaters to Pass it to Module 
#include <linux/fs.h> //fs -> <file structure> To Create Major Number and Minor Number
#include <linux/cdev.h>
#include <linux/gpio.h>

#define SIZE 3
#define LED_PIN_NUM 23
#define Btn_PIN_NUM 3

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sami Selim");
MODULE_DESCRIPTION("Psuedo device driver to print hello .");

struct class * my_class;
struct device * my_device;
dev_t device_number ;
static unsigned char Data[SIZE] = "";

static int Driver_open (struct inode * device_file, struct file * instance)
{
    printk("%s Was Called \n", __FUNCTION__);
    return 0;
}

int Driver_release (struct inode *device_file, struct file *instance)
{
    printk("%s Was Called \n", __FUNCTION__);
    return 0;
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

    tmp[0] = gpio_get_value(Btn_PIN_NUM) + 48;
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
    
    printk(KERN_INFO "Data read : %s\n", print_buffer);
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
            break;
        case '1':
            gpio_set_value(LED_PIN_NUM , 1);
            break;
    }

    printk("Data Written  : %s\n" , Data);
    return (ssize_t)count;

}

struct file_operations fops =
{
    .owner=THIS_MODULE,
    .open=Driver_open,
    .release=Driver_release,
    .read=Driver_Read,
    .write=Driver_Write
};

struct cdev st_characterDevice;
static int __init DD_HelloWorld(void)
{

    int ret;
   
    //ret = register_chrdev(major_number , "MyModuleDriver" ,&fops);

    ret= alloc_chrdev_region(&device_number , 0 , 1 ,"SamiGPIO");

    if(0 == ret)
    {
        printk("%s retval=0 , registered Device Number Major : %d , Minor: %d\n" , __FUNCTION__ , MAJOR(device_number) , MINOR(device_number));
    }
    else
    {
         printk("Couldn't register device number !! .. \n");
         return -1;
    }
    // Define Device as Character Device 
    cdev_init(&st_characterDevice ,&fops);
    ret = cdev_add(&st_characterDevice , device_number,1);
    if(ret !=0)
    {
        printk("Registering of Device Failed \n");
        goto CHARACTER_ERROR;
       
    }

    /*  Creating Driver Class dev file */
    if((my_class = class_create( THIS_MODULE, "test_class")) == NULL)
    {
        printk("Device Can't be created !\n");
        goto CLASS_ERROR; 
        
    }
    my_device = device_create(my_class , NULL , device_number , NULL , "SamiGPIO_Dev");
    if(my_device == NULL)
    {
        printk("Device class Can't be created \n");
        goto DEVICE_ERROR;
      
    }
    printk("Device Driver Created Successfully \n");

   
    if(gpio_request(LED_PIN_NUM , "GPIO23"))
    {
        printk(KERN_ERR "Can not request LED PIN\n");
        goto GPIO_REQ_ERROR;
    }
    if(gpio_direction_output(LED_PIN_NUM ,0) != 0)
    {
        printk(KERN_ERR "Can not Make Direction output for LED PIN\n");
        goto MAKE_DIR_ERROR;
    }

    if(gpio_request(Btn_PIN_NUM , "GPIO3"))
    {
        printk(KERN_ERR "Can not request Btn PIN\n");
        goto MAKE_DIR_ERROR;
    }
    if(gpio_direction_input(Btn_PIN_NUM) != 0)
    {
        printk(KERN_ERR "Can not Make Direction output for Btn PIN\n");
        goto MAKE_BTN_ERROR;
    }

    return 0;
    MAKE_BTN_ERROR:
        gpio_free(Btn_PIN_NUM);
    MAKE_DIR_ERROR:
        gpio_free(LED_PIN_NUM);
    GPIO_REQ_ERROR:
        device_destroy(my_class,device_number);
    DEVICE_ERROR:
        class_destroy(my_class);
    CLASS_ERROR:
        cdev_del(&st_characterDevice);    
    CHARACTER_ERROR:
        unregister_chrdev_region(device_number , 1);
        return -1;

}


static void __exit DD_Goodby(void)
{
    //unregister_chrdev(major_number , "MyModuleDriver");
    gpio_set_value(LED_PIN_NUM ,0);
    gpio_free(LED_PIN_NUM);
    gpio_free(Btn_PIN_NUM);

    cdev_del(&st_characterDevice);
    device_destroy(my_class,device_number);
    class_destroy(my_class);
    unregister_chrdev_region(device_number , 1);
    printk("Goodbye\n");
      
}


module_init(DD_HelloWorld);
module_exit(DD_Goodby);