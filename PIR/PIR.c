#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

// GPIO number for the PIR sensor (change this according to your setup)
#define PIR_GPIO 17

// IRQ handler function
static irqreturn_t pir_irq_handler(int irq, void *dev_id)
{
    // Read the state of the GPIO pin (HIGH = motion detected, LOW = no motion)
    int state = gpio_get_value(PIR_GPIO);
    printk(KERN_INFO "PIR sensor: Motion %s\n", state ? "detected" : "not detected");
    return IRQ_HANDLED;
}

static int __init pir_sensor_init(void)
{
    int irq;
    int result;

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
}

static void __exit pir_sensor_exit(void)
{
    int irq = gpio_to_irq(PIR_GPIO);

    // Free the IRQ
    free_irq(irq, NULL);

    // Free the GPIO
    gpio_free(PIR_GPIO);

    printk(KERN_INFO "PIR sensor module removed\n");
}

module_init(pir_sensor_init);
module_exit(pir_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sami Selim");
MODULE_DESCRIPTION("PIR sensor kernel module");
