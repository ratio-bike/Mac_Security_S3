#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9a31bb74, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0x4c4fef19, "kernel_stack" },
	{ 0x830e547b, "ioremap_prot" },
	{ 0x27e3ef6b, "device_destroy" },
	{ 0x9cc5adfc, "__register_chrdev" },
	{ 0x37013607, "mutex_unlock" },
	{ 0x54efb5d6, "cpu_number" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xfb578fc5, "memset" },
	{ 0xf10de535, "ioread8" },
	{ 0x27e1a049, "printk" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x7da7be30, "mutex_lock" },
	{ 0xd4f29297, "device_create" },
	{ 0xe007de41, "kallsyms_lookup_name" },
	{ 0xbfe4784c, "efi" },
	{ 0xc6cbbc89, "capable" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x727c4f3, "iowrite8" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0x8c183cbe, "iowrite16" },
	{ 0x37a0cba, "kfree" },
	{ 0xedc03953, "iounmap" },
	{ 0xf3248a6e, "class_destroy" },
	{ 0xc5534d64, "ioread16" },
	{ 0xf1a12f00, "directly_mappable_cdev_bdi" },
	{ 0x436c2179, "iowrite32" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xe5d95985, "param_ops_ulong" },
	{ 0x25827ecd, "__class_create" },
	{ 0x6228c21f, "smp_call_function_single" },
	{ 0xe484e35f, "ioread32" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

