#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/sched.h>


#include <linux/kernel_stat.h>
#include <linux/mman.h>  
#include <linux/swap.h>  
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/ksm.h>
#include <linux/rmap.h>  
#include <linux/writeback.h>
#include <linux/memcontrol.h>
#include <linux/mmu_notifier.h>
#include <linux/kallsyms.h>
#include <linux/swapops.h>
#include <linux/elf.h>   

#include <asm/io.h>      
#include <asm/pgalloc.h> 
#include <asm/uaccess.h> 
#include <asm/tlb.h>     
#include <asm/tlbflush.h>
#include <asm/pgtable.h> 
#include <trace/events/kmem.h>        



void pte_test(void)
{
	struct task_struct *task;
        pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep, pte;
	int i;

	task = current_thread_info()->task;


	printk("comm = %s pid = %d", task->comm, task->pid);
	printk("\n");

	pgd = pgd_offset(task->mm, (unsigned long)0x0000000000400c40);

	printk("pgd = %#lx\n",  *pgd);

	
	
	if (pgd_none(*pgd) || pgd_bad(*pgd))
                return;

	pud = pud_offset(pgd,  0x0000000000400c40); 

	if (pud_none(*pud) || pud_bad(*pud))
                return; 

        pmd = pmd_offset(pgd, 0x0000000000400c40);

	if (pmd_none(*pmd) || pmd_bad(*pmd))
                return;

        ptep = pte_offset_map(pmd, 0x0000000000400c40);
	if (!ptep)
		return;	

     	pte = *ptep;

	printk("pte = %#lx \n",  pte);

	task->state = TASK_UNINTERRUPTIBLE;

	schedule();
	
	return;


}
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
	pte_test();
//	mem_map_test();

//	kmalloc_test();
//	vmalloc_test();
//	alloc_pages_test();
//	get_free_pages_test();

	return 0;
}

void memtest_exit(void)
{
	printk("Module Memory Test End \n");
}

module_init(memtest_init);
module_exit(memtest_exit);

MODULE_LICENSE("Dual BSD/GPL");

