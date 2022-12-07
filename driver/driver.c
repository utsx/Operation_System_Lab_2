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
#include "driver_structs.h"
#include <linux/ptrace.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitrii Aleksandrov");

static int pid = 0;
static int struct_id = 0;

static struct proc_dir_entry *proc_root;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

static char procfs_buffer[PROCFS_MAX_SIZE];

static unsigned long procfs_buffer_size = 0;

//module parameters

static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

static const struct proc_ops proc_ops = {
    .proc_read = lab_driver_read,
    .proc_write = lab_driver_write
};


static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if(task == NULL){
        printk(KERN_INFO "lab_driver: task is null");
        return -EFAULT;
    }
    switch (struct_id) {
        case STRUCT_PCI_DEV:
            struct pci_dev *dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
            struct user_pci_dev user_pci_dev;
            user_pci_dev = (struct user_pci_dev){
                .device = dev->device,
                .vendor = dev->vendor,
                .subsystem_device = dev->subsystem_device,
                .subsystem_vendor = dev->subsystem_vendor,
            };
            if(copy_to_user(buf, &user_pci_dev, sizeof(struct user_pci_dev))){
                printk(KERN_INFO "lab_driver: PCI_DEV copy_to_user failed");
                return -EFAULT;
            }
            break;
        case STRUCT_INODE:
            printk(KERN_INFO "lab_driver: write inode");
            struct mm_struct *mm = task->mm;
            struct vm_area_struct *vma = mm->mmap;
            struct inode *inode = vma->vm_file->f_inode;
            struct user_inode user_inode = (struct user_inode){
                .i_ino = inode->i_ino,
                .i_mode = inode->i_mode
            };
            if(copy_to_user(buf, &user_inode, sizeof(struct user_inode))){
                printk(KERN_INFO "lab_driver: INODE copy_to_user failed");
                return -EFAULT;
            }
            break;
    }
    return SUCCESS;
}


static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int num_of_args, a, b, c;
    procfs_buffer_size = count;
    if(*ppos > 0 || count > PROCFS_MAX_SIZE){
        pr_info("lab_driver: write error");
        return -EFAULT;
    }
    if(copy_from_user(procfs_buffer, buf, count)){
        printk(KERN_INFO "lab_driver: copy from user error");
        return -EFAULT;
    }
    num_of_args = sscanf(procfs_buffer, "%d %d", &a, &b);
    if(num_of_args != 2){
        printk(KERN_INFO "lab_driver: sscanf error");
        return -EFAULT;
    }
    if(a != STRUCT_PCI_DEV && a != STRUCT_INODE){
        printk(KERN_INFO "lab_driver: wrong struct id");
        return -EFAULT;
    }
    struct_id = a;
    pid = b;
    c = strlen(procfs_buffer);
    *ppos = c;
    return c;
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