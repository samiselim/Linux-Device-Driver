#include <linux/init.h>
#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>

MODULE_AUTHOR("Sami Abdelaziz Selim");
MODULE_LICENSE("Dual BSD/GPL");


/****** You Must Enable PWM Device Tree for Raspberry pi 
 * go to /boot/config.txt
 * Add (dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4) to this file 
*/


#define PWM_CHANNEL 0
#define SIZE 255
#define Pwm_Period 1000000000

struct class * my_class;
struct device * my_device;
struct cdev st_characterDevice;
dev_t device_number ;
static unsigned char Data[SIZE] = "";

static struct pwm_device *pwm_dev;
u32 pwm_ton = 500000000; //500 MHZ
unsigned long value;





/****** File Operations APIs *******/
ssize_t Driver_Read (struct file *file, char __user * user_buffer, size_t count, loff_t *offset)
{
    int remaining_bytes;
    char *print_buffer;

    printk(KERN_INFO "------ Reading Data ----- \n");

    remaining_bytes = SIZE - *offset;
    if (remaining_bytes <= 0)
        return 0;  // End of file reached.
    
    if (count > remaining_bytes)
        count = remaining_bytes;


    if (copy_to_user(user_buffer, &Data[*offset], count))
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
    
    printk(KERN_INFO "Read data: %s\n", print_buffer);
    kfree(print_buffer);

    return (ssize_t)count;
}
ssize_t Driver_Write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset)
{
    int remaining_bytes;
    printk(KERN_INFO "------ Writing Data ----- \n");

    remaining_bytes = SIZE - *offset;
    if (remaining_bytes <= 0)
        return 0;  // End of file reached.
    if (count > remaining_bytes)
        count = remaining_bytes;  // Don't write more than what's available.

    /*************** We will take duty cycle of the PWM from the user  *********/
    /***** Convert String which the user Enter it to Integger *********/

    /*Args
    * 1) String Data 
    * 2) count of Characters
    * 3) Base Number ex: 0x --> hexa -- 0b --> binary -- 0 which automatic Detect 
    */
    if(kstrtol_from_user(user_buffer , count-1 , 0 , &value) != 0)
        return -1;

    printk("New Duty Cycle in Milli_S : %ld\n" , value);
    *offset += count;  // Update the file position.

    if(value > (Pwm_Period/1000000))
    {
        printk( KERN_ERR "Invalid Duty Cycle Enter Value less than PWM Period\n");
    }
    else
    {
        pwm_config(pwm_dev , value*1000000 , Pwm_Period);    
    }
    
    return (ssize_t)count;

}
/**************************************/
struct file_operations pwm_fops =
{
    .owner=THIS_MODULE,
    .read=Driver_Read,
    .write=Driver_Write
};

static int pwm_init(void)
{
    int ret;
	printk(KERN_INFO "Hello to PWM Device Driver for Raspberrypi 3 Kernel \n");
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
    cdev_init(&st_characterDevice ,&pwm_fops);
    ret = cdev_add(&st_characterDevice , device_number,1);
    if(ret !=0)
    {
        printk("Registering of Device Failed \n");
        goto CHARACTER_ERROR;
       
    }
    /*  Creating Driver Class dev file */
    if((my_class = class_create(THIS_MODULE , "pwm_class")) == NULL)
    {
        printk("Device Can't be created !\n");
        goto CLASS_ERROR;
    }
    my_device = device_create(my_class , NULL , device_number , NULL , "pwm_device");
    if(my_device == NULL)
    {
        printk("Device class Can't be created \n");
        goto DEVICE_ERROR;
    }
    pwm_dev = pwm_request(PWM_CHANNEL, "pwm_driver");
    if(IS_ERR(pwm_dev))
    {
        printk( KERN_ERR "Failed to request PWM channel %d\n", PWM_CHANNEL);
        goto PWM_ERROR;
    }
    pwm_config(pwm_dev , pwm_ton ,Pwm_Period);
    pwm_enable(pwm_dev);
   
    printk("PWM Device Driver Created Successfully \n");

    return 0;
    PWM_ERROR:
        device_destroy(my_class,device_number);
    DEVICE_ERROR:
        class_destroy(my_class);
    CLASS_ERROR:
        cdev_del(&st_characterDevice);    
    CHARACTER_ERROR:
        unregister_chrdev_region(device_number , 1);
        return -1;
}

static void pwm_cleanup(void)
{
	printk(KERN_INFO "Exit PWM Device Driver \n");
    pwm_disable(pwm_dev);
    pwm_free(pwm_dev);
    cdev_del(&st_characterDevice);
    device_destroy(my_class,device_number);
    class_destroy(my_class);
    unregister_chrdev_region(device_number , 1);
}




module_init(pwm_init);
module_exit(pwm_cleanup);