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
static char procfs_buffer[PROCFS_MAX_SIZE];

static struct proc_dir_entry *proc_root;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

static unsigned long procfs_buffer_size = 0;

//module parameters

static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

//service functions

static struct user_pci_dev get_pci_dev(void);
static int write_pci_dev(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos);
static int write_inode(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos);


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = lab_driver_read,
    .write = lab_driver_write
};

static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int len = 0;
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if(*ppos > 0 || count < procfs_buffer_size){
        printk(KERN_INFO "lab_driver: read error");
        return -EFAULT;
    }
    if(task == NULL){
        len += sprintf(procfs_buffer,"No such process with pid = %d\n", pid);
        if(copy_to_user(buf, procfs_buffer, len)){
            printk(KERN_INFO "lab_driver: read error");
            return -EFAULT;
        }
        *ppos = len;
        return len;
    }
    switch (struct_id) {
        case STRUCT_PCI_DEV:
            len = write_pci_dev(task, buf, count, ppos);
            break;
        case STRUCT_INODE:
            len = write_inode(task, buf, count, ppos);
            break;
    }
    *ppos = len;
    return len;
}


static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int num_of_args, a, b, c;
    procfs_buffer_size = count;
    if(*ppos > 0 || count > PROCFS_MAX_SIZE){
        printk(KERN_INFO "lab_driver: write error\n");
        return -EFAULT;
    }
    if(copy_from_user(procfs_buffer, buf, count)){
        printk(KERN_INFO "Error while copying from user");
        return -EFAULT;
    }
    num_of_args = sscanf(procfs_buffer, "%d %d", &a, &b);
    if(num_of_args != 2){
        printk(KERN_INFO "Wrong number of arguments\n");
        return -EFAULT;
    }
    struct_id = a;
    pid = b;
    c = strlen(procfs_buffer);
    *ppos = c;
    return c;
}


// pci_dev functions


//get pci_dev struct and return it as user_pci_dev
struct user_pci_dev get_pci_dev()
{
    struct pci_dev *dev = NULL;
    struct user_pci_dev user_pci_dev;
    dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
    user_pci_dev = (struct user_pci_dev){
        .device = dev->device,
        .vendor = dev->vendor,
        .subsystem_device = dev->subsystem_device,
        .subsystem_vendor = dev->subsystem_vendor,
    };
    return user_pci_dev;
}


//write user_pci_dev struct to user
static int write_pci_dev(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos) {
    struct user_pci_dev user_dev = get_pci_dev();
    size_t len = 0;
    len += sprintf(procfs_buffer + len, "vendor: %d\n", user_dev.vendor);
    len += sprintf(procfs_buffer + len, "device: %d\n", user_dev.device);
    len += sprintf(procfs_buffer + len, "subsystem_vendor: %d\n", user_dev.subsystem_vendor);
    len += sprintf(procfs_buffer + len, "subsystem_device: %d\n", user_dev.subsystem_device);
    if (*ppos >= len || copy_to_user(buf, procfs_buffer, len)){
        printk(KERN_INFO "lab_driver: write_pci_dev() - copy_to_user() failed\r");
        return -EFAULT;
    }
    *ppos += len;
    return len;
}


//inode functions

//write user_inode struct to user

static int write_inode(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos){
    struct mm_struct *mm = task->mm;
    struct vm_area_struct *vma = mm->mmap;
    struct inode *inode = vma->vm_file->f_inode;
    size_t len = 0;
    len+= sprintf(procfs_buffer + len, "i_ino: %ld\n", inode->i_ino);
    len+= sprintf(procfs_buffer + len, "i_mode: %d\n", inode->i_mode);
    len+= sprintf(procfs_buffer + len, "i_nlink: %d\n", inode->i_nlink);
    len+= sprintf(procfs_buffer + len, "i_rdev: %d\n", inode->i_rdev);
    len+= sprintf(procfs_buffer + len, "i_size: %lld\n", inode->i_size);
    len+= sprintf(procfs_buffer + len, "i_blocks: %lld\n", inode->i_blocks);
    if (*ppos >= len || copy_to_user(buf, procfs_buffer, len)){
        printk(KERN_INFO "lab_driver: write_inode() - copy_to_user() failed\r");
        return -EFAULT;
    }
    *ppos += len;
    return len;
}

//module init and exit functions

static int __init lab_driver_init(void)
{
    printk(KERN_INFO "lab_driver: init()\r");
    proc_root = proc_create("lab_driver", 0666, NULL, &fops);
    if(proc_root == NULL){
        proc_remove(proc_root);
        printk(KERN_INFO "lab_driver: init() - proc_create() failed\r");
        return -ENOMEM;
    }
    return SUCCESS;
}

static void __exit lab_driver_exit(void)
{
    printk(KERN_INFO "lab_driver: exit()\r");
    proc_remove(proc_root);
}

module_init(lab_driver_init);
module_exit(lab_driver_exit);