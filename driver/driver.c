#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include "../driver_structs.h"
#include <linux/ptrace.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitrii Aleksandrov");


static struct proc_dir_entry *proc_root;

struct answer answer;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

//module parameters

static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

enum {
    DEVICE_NOT_USED = 0,
    DEVICE_OPEN = 1,
};

static atomic_t already_open = ATOMIC_INIT(DEVICE_NOT_USED);


static const struct proc_ops proc_ops = {
    .proc_read = lab_driver_read,
    .proc_write = lab_driver_write
};


static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct task_struct *task;
    atomic_set(&already_open, DEVICE_NOT_USED);
    task = get_pid_task(find_vpid(answer.pid), PIDTYPE_PID);
    if(task == NULL){
        printk(KERN_INFO "lab_driver: task is null");
        answer = (struct answer){
            .pid = -1};
    }
    else{
        struct inode *inode;
        struct pci_dev *dev;
        if(!task->mm){
            printk(KERN_INFO "lab_driver: task->mm is null");
            answer = (struct answer){
                .pid = -1};
        }
        else{
            printk(KERN_INFO "lab_driver: task->mm is not null");
            inode = task->mm->mmap->vm_file->f_inode;
            dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
                answer = (struct answer){
                        .pid = answer.pid,
                        .pci_dev = (struct user_pci_dev){.vendor = dev->vendor,
                                .device = dev->device,
                                .subsystem_vendor = dev->subsystem_vendor,
                                .subsystem_device = dev->subsystem_device},
                        .inode = (struct user_inode){.i_ino = inode->i_ino,
                            .i_mode = inode->i_mode,
                            .i_flags = inode->i_flags,
                            .i_size = inode->i_size,
                            .i_blocks = inode->i_blocks,
                            .i_uid = inode->i_uid.val,
                            .i_gid = inode->i_gid.val}
                };
        }
    }
    if(copy_to_user(buf, &answer, sizeof(struct answer)) != 0){
        printk(KERN_INFO "lab_driver: copy_to_user failed");
        return -1;
    }
    return SUCCESS;
}


static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    if (atomic_cmpxchg(&already_open, DEVICE_NOT_USED, DEVICE_OPEN)) {
        return -EBUSY;
    }

    if(*ppos > 0 || count > PROCFS_MAX_SIZE){
        printk(KERN_INFO "lab_driver: write failed");
        return -EFAULT;
    }
    if(copy_from_user(&answer, buf, count) != 0){
        printk(KERN_INFO "lab_driver: copy_from_user failed");
        return -EFAULT;
    }
    return SUCCESS;
}

//module init and exit functions

static int __init lab_driver_init(void)
{

    proc_root = proc_create(PROCFS_NAME, 0666, NULL, &proc_ops);
    if(proc_root == NULL){
        proc_remove(proc_root);
        printk(KERN_ALERT "lab_driver: Could not initialize /proc/%s", PROCFS_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "lab_driver: module loaded\n");
    return SUCCESS;
}

static void __exit lab_driver_exit(void)
{
    proc_remove(proc_root);
    printk(KERN_INFO "lab_driver: module unloaded\n");
}

module_init(lab_driver_init);
module_exit(lab_driver_exit);