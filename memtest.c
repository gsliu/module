#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

void kmalloc_test(void)
{
	char *buff;

	printk("kmalloc_test\n");

	buff = kmalloc(1024, GFP_KERNEL);
	if(buff != NULL)
	{
		sprintf(buff, "test memory \n");
		printk(buff);

		kfree(buff);
	}

	buff = kmalloc(32*PAGE_SIZE, GFP_KERNEL);
	if(buff != NULL)
	{
		printk("Big Memory Ok \n");
		kfree(buff);
	}
}

void vmalloc_test(void)
{
	char *buff;

	printk("vmalloc_test \n");

	buff = vmalloc(33*PAGE_SIZE);
	if(buff != NULL)
	{
		sprintf(buff, "vmalloc test ok \n");
		printk(buff);

		vfree(buff);
	}
}
void alloc_pages_test(void)
{
	struct page *page;
	unsigned long address;
	char *buff;
	int i;

	for( i = 0 ; i< 1024*512; i++)
	{
		page = alloc_pages(GFP_KERNEL, 1);
	
     		if (!page)
              		return ;
	
        	address =  (unsigned long) page_address(page);
		printk("page address 	 = %#lx\n", page);
		printk("virtual address = %#lx\n", address);
	}
	


	 return;

}
	

void get_free_pages_test(void)
{
	char *buff;
	int order;

	printk("get_free_pages test \n");

	order = get_order(8192*10);
	buff = __get_free_pages(GFP_KERNEL, order);
	if(buff != NULL)
	{
		sprintf(buff, "__get_free_pages test ok [%d] \n", order);
		printk("%#lx\n", buff);
		printk(buff);

		free_pages(buff, order);
	}
}
void mem_map_test()
{
	struct page * mem_map;
	struct page * page;
	char * c;
	int i;

	mem_map = 0xffffea0000038000;
	printk("mem_map address = %#lx\n", (unsigned long) mem_map);

	//page = mem_map[0];

	for( i = 0; i< 1024;i++)
	{
		page = &mem_map[i];
		printk(" mem_map[%d] flag : %#lx \n", i, page->flags);
	}

	c =(char *) (0x0000000000400c40);

	for( i = 0; i< 100; i++)
	{
		printk("%c", c[i]);
	}
	printk("\n");

	return;
}


int memtest_init(void)
{
	char *data;

	printk("Module Memory Test \n");
//	mem_map_test();

	kmalloc_test();
	vmalloc_test();
	alloc_pages_test();
	get_free_pages_test();

	return 0;
}

void memtest_exit(void)
{
	printk("Module Memory Test End \n");
}

module_init(memtest_init);
module_exit(memtest_exit);

MODULE_LICENSE("Dual BSD/GPL");

