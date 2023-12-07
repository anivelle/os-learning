#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/param.h>
#include <linux/sched.h>

#define BUF_SIZE 128
#define PROC_NAME "task_info"

static struct task_struct *task;

// Skeleton straight from the book
// One change - usr_buf has to be a const char
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count,
                   loff_t *pos) {
    int rv = 0;
    char *k_mem;

    // Allocate kernel memory
    k_mem = kmalloc(count, GFP_KERNEL);

    // Copy from user space to kernel memory
    rv = copy_from_user(k_mem, usr_buf, count);
    long task_pid = 0;
    if (kstrtol(k_mem, 10, &task_pid) != 0) {
        printk(KERN_ERR "Invalid PID input\n");
        return -1;
    }

    struct pid *proc = find_vpid(task_pid);

    task = pid_task(proc, PIDTYPE_PID);

    // Return kernel memory
    kfree(k_mem);
    kfree(proc);

    return count;
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos) {
    int rv = 0;
    char buffer[BUF_SIZE];
    static int completed = 0;

    if (completed) {
        completed = 0;
        kfree(task);
        return 0;
    }

    if (task == NULL) {
        printk(KERN_ERR "Invalid task\n");
        completed = 1;
        return -1;
    }

    completed = 1;
    struct thread_info info = task->thread_info;
    struct pid *task_pid = task->thread_pid;
    
    // I don't know if info.status is correct but info.flags doesn't feel right
    // either. Maybe a status of 0 is good idk
    rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%ld]\n", task->comm,
    task_pid->numbers[0].nr, info.flags);
    // This function returns the number of bytes not copied, but it looks like
    // you can't return the value on the first go if you want the buffer to be
    // copied.
    copy_to_user(usr_buf, buffer, rv);
    return rv;
}

static struct proc_ops ops = {.proc_read = proc_read, .proc_write = proc_write};

int proc_init(void) {
    proc_create(PROC_NAME, 0777, NULL, &ops);
    return 0;
}

void proc_exit(void) { remove_proc_entry(PROC_NAME, NULL); }

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task Info Module");
MODULE_AUTHOR("Me");
