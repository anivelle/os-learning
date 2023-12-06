#include <linux/init.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/param.h>

#define BUF_SIZE 128
#define PROC_NAME "pid"

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

    printk(KERN_INFO "%s\n", k_mem);

    // Return kernel memory
    kfree(k_mem);

    return count;
}

static struct proc_ops ops = {.proc_write = proc_write};

int proc_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &ops);
    return 0;
}

void proc_exit(void) { remove_proc_entry(PROC_NAME, NULL); }

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task Info Module");
MODULE_AUTHOR("Me");
