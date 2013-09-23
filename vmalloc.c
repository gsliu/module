#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
MODULE_LICENSE("GPL");
unsigned char *pagemem;
unsigned char *kmallocmem;
unsigned char *vmallocmem;

int __init mem_module_init(void)
{
//最好每次内存申请都检查申请是否成功
//下面这段仅仅作为演示的代码没有检查
//	pagemem = (unsigned char*)get_free_page(0);
	//printk("<1>pagemem addr=%x", pagemem);

	//kmallocmem = (unsigned char*)kmalloc(100, 0);
	//printk("<1>kmallocmem addr=%x", kmallocmem);

	vmallocmem = (unsigned char*)vmalloc(1000000);
	printk("<1>vmallocmem addr=%x", vmallocmem);

	return 0;
}

void __exit mem_module_exit(void)
{
	free_page(pagemem);
	kfree(kmallocmem);
	vfree(vmallocmem);
}

module_init(mem_module_init);
module_exit(mem_module_exit);
