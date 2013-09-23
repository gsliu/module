#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>	/* For current */
#include <linux/tty.h>		/* For the tty declarations */
#include <linux/version.h>	/* For LINUX_VERSION_CODE */
#ifndef SLEEP_MILLI_SEC

#define SLEEP_MILLI_SEC(nMilliSec)/
#endif

static struct task_struct * MyThread = NULL;

static int MyPrintk(void *data)
{
	char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);
	struct task_struct * task = &init_task;
	memset(mydata,'\0',strlen(data)+1);
	strncpy(mydata,data,strlen(data));
	while(!kthread_should_stop())
	{
	//	SLEEP_MILLI_SEC(1000);
		do { 
	        long timeout = (1000) * HZ / 1000; 
	        while(timeout > 0) 
	        { 
	             timeout = schedule_timeout(timeout); 
	        } 
	        }while(0);
	
		printk("%s\n",mydata);

		do{
			if( task->pid == 1)
			{
		//		printk("%s\t%d\t%d\t%d\t%lu\t%d\t %lu\n",task->comm, task->utime,task->stime,task->pid,task->state,task->parent->pid, (unsigned long)task->mm);	
				printk("task 1 mm ->pgd = %lu   mm->rss = %lu\n", task->mm->pgd, task->mm->_anon_rss);
				//printk("%s\t%d\t%lu\t%d\t %lu\n",task->comm, task->utime,task->pid,task->state,task->parent->pid, (unsigned long)task->mm);	
			}	
		}while((task=next_task(task))!=&init_task);
		printk("#######################################################\n\n");
	}
	
	kfree(mydata);
	return 0;
}
static int __init init_kthread(void)
{
	MyThread = kthread_run(MyPrintk,"hello world","mythread");
	return 0;
}
static void __exit exit_kthread(void)
{
	if(MyThread)
	{
		printk("stop MyThread/n");
		kthread_stop(MyThread);
	}
}
module_init(init_kthread);
module_exit(exit_kthread);
MODULE_AUTHOR("GSLIU");
