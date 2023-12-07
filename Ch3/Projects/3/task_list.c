#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/param.h>
#include <linux/sched/task.h>

#define BUF_SIZE 128
#define PROC_NAME "task_info"

struct task_struct *task;
struct list_head *list;

// Prints just in order
int module_linear(void) {
    for_each_process(task) {
        printk(KERN_INFO "command [%s] pid [%d] status [%d]", task->comm,
               task->thread_pid->numbers[0].nr, task->thread_info.status);
    }
    return 0;
}

// Proceeds depth-first
int module_depth(void) {
    list_for_each(list, &init_task.children) {
        task = list_entry(list, struct task_struct, sibling);
        printk(KERN_INFO "command [%s] pid [%d] status [%d]", task->comm,
               task->thread_pid->numbers[0].nr, task->thread_info.status);
    }
    return 0;
}

void proc_exit(void) {}
module_init(module_depth);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task List Module");
MODULE_AUTHOR("Me");
