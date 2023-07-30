#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x882ce6fa, "gpio_to_desc" },
	{ 0x907b1f36, "gpiod_get_raw_value" },
	{ 0x92997ed8, "_printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x7fb98451, "cdev_init" },
	{ 0x91128fc8, "cdev_add" },
	{ 0x8d30dba0, "__class_create" },
	{ 0xbf3f457f, "device_create" },
	{ 0x7391ca5d, "class_destroy" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xc62a4969, "gpiod_direction_input" },
	{ 0xfe990052, "gpio_free" },
	{ 0x183a612f, "gpiod_direction_output_raw" },
	{ 0x2bf839e4, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x43dd1c15, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x54ae6ab6, "device_destroy" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x37a0cba, "kfree" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xdcb764ad, "memset" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0xd1cb405a, "gpiod_set_raw_value" },
	{ 0x8f80e6e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "45FD05828F6189F0D5B1B2E");
