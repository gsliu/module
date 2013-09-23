#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

static int __init init_function(void)
{
	printk(KERN_INFO "test module loading\n");
	return 0;
}
static void __exit exit_function(void)
{
	printk(KERN_INFO "test  unloading\n");
}
module_init(init_function);
modue_exit(exit_function);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("gsliu");
MODULE_DESCRIPTION(" test module");
