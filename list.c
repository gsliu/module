#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Kozo");

struct my_struct{
	const char *name;
	int index;
	struct list_head list;
};

//struct list_head head = LIST_HEAD_INIT(head);
LIST_HEAD(head);
static int __init demo_init(void)
{
	struct my_struct *task1;
	struct my_struct *task2;
	struct my_struct *task3;

	task1 = (struct my_struct *)kmalloc(sizeof(struct my_struct),GFP_KERNEL);
	task2 = (struct my_struct *)kmalloc(sizeof(struct my_struct),GFP_KERNEL);
	task3 = (struct my_struct *)kmalloc(sizeof(struct my_struct),GFP_KERNEL);

	task1->name="One";
	task1->index=1;
	task2->name="Two";
	task2->index=2;
	task3->name="Three";
	task3->index=3;
	
	list_add(&task1->list,&head);
	list_add(&task2->list,&head);
	list_add(&task3->list,&head);

	printk(KERN_DEBUG"added node\n");
	return 0;
}

static void __exit demo_exit(void)
{
	struct my_struct *task;
	struct my_struct *tmp = NULL;
	list_for_each_entry(task,&head,list){
		printk(KERN_DEBUG"name:%s,index:%d\n",task->name,task->index);
		if(tmp == NULL)
			tmp = task;   /*Ê¹ÓÃlist_for_each_entryÊ±£¬²»ÄÜÉŸ³ý¶ÁÈ¡µœœÚµã£¬µ«¿ÉÒÔÉŸ³ýžÃœÚµãµÄÇ°œÚµã»òºóœÚµã*/
		else
		{
			list_del(&tmp->list);
			kfree(tmp);
			tmp = task;
		}
	}
		list_del(&tmp->list);
		kfree(tmp);
	/*È·ÈÏÒÑŸ­ÉŸ³ýËùÓÐÌíŒÓµÄœÚµã*/
	list_for_each_entry(task,&head,list){
		printk(KERN_DEBUG"name:%s,index:%d\n",task->name,task->index);
	}
	printk("drop node,and free memory\n");
}
module_init(demo_init);
module_exit(demo_exit);
