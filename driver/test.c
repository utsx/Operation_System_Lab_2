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
#include "lab_structs.h"
#include <linux/ptrace.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitrii Aleksandrov");

static int pid = 0;
static int struct_id = 1;
static unsigned long addr = 0;

static char procfs_buffer[PROCFS_MAX_SIZE];

static struct proc_dir_entry *proc_root;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

static unsigned long procfs_buffer_size = 0;

static int lab_driver_open(struct inode *inode, struct file *file);
static int lab_driver_release(struct inode *inode, struct file *file);
static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static struct user_pci_dev get_pci_dev(void);
static int write_pci_dev(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos);
static struct user_inode my_get_inode_by_pid(void);
static int write_inode(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos);


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = lab_driver_read,
    .write = lab_driver_write,
    .open = lab_driver_open,
    .release = lab_driver_release,
};

static int lab_driver_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "lab_driver: open()\r");
    return SUCCESS;
}

static int lab_driver_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "lab_driver: release()\r");
    return SUCCESS;
}
static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    if(count < PROCFS_MAX_SIZE)
        return -EFAULT;
    if(pid){
        struct task_struct *task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
        if(task == NULL){
            printk(KERN_INFO "lab_driver: read() - task is NULL\r");
            return -EFAULT;
        }
        if(struct_id == STRUCT_PCI_DEV){
            printk(KERN_INFO "lab_driver: read() - struct_id == STRUCT_PCI_DEV\r");
            return write_pci_dev(task, buf, count, ppos);
        }
        if(struct_id == STRUCT_INODE){
            printk(KERN_INFO "lab_driver: read() - struct_id == STRUCT_INODE\r");
            return write_inode(task, buf, count, ppos);
        }
        }
        return -EFAULT;
}




static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
int num_of_args, a, b;
procfs_buffer_size = count;
if(procfs_buffer_size > PROCFS_MAX_SIZE) {
procfs_buffer_size = PROCFS_MAX_SIZE;
}
if(copy_from_user(procfs_buffer, buf, procfs_buffer_size)) {
return -EFAULT;
}

num_of_args = sscanf(procfs_buffer, "%d %d", &a, &b);
if(num_of_args != 2){
printk(KERN_INFO "lab_driver: write() - wrong number of arguments\r");
return -EFAULT;
}
struct_id = a;
pid = b;
return procfs_buffer_size;
}


// pci_dev functions


//get pci_dev struct and return it as user_pci_dev
struct user_pci_dev get_pci_dev()
{
    struct pci_dev *dev = NULL;
    dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
    struct user_pci_dev user_dev = (struct user_pci_dev){
        .device = dev->device,
        .vendor = dev->vendor,
        .subsystem_device = dev->subsystem_device,
        .subsystem_vendor = dev->subsystem_vendor,
    };
    return user_dev;
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
        printk(KERN_INFO "lab_driver: read() - copy_to_user() failed\r");
        return -EFAULT;
    }
    *ppos += len;
    return len;
}


//inode functions

static struct user_inode my_get_inode_by_pid(){
    struct task_struct *task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    struct mm_struct *mm = task->mm;
    struct vm_area_struct *vma = mm->mmap;
    struct inode *inode = vma->vm_file->f_inode;
    struct user_inode user_inode = (struct user_inode){
        .i_bytes = inode->i_bytes,
        .i_flags = inode->i_flags,
    };
    return user_inode;
}

//write user_inode struct to user

static int write_inode(struct task_struct *task, char __user *buf, size_t count, loff_t *ppos){
    struct user_inode user_inode = my_get_inode_by_pid();
    printk(KERN_INFO "lab_driver: read() - i_bytes: %d\r", user_inode.i_bytes);
    size_t len = 0;
    len += sprintf(procfs_buffer + len, "i_bytes: %d\n", user_inode.i_bytes);
    len += sprintf(procfs_buffer + len, "i_flags: %d\n", user_inode.i_flags);
    if (*ppos >= len || copy_to_user(buf, procfs_buffer, len)){
        printk(KERN_INFO "lab_driver: read() - copy_to_user() failed\r");
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
    if (register_chrdev(0, "lab_driver", &fops) < 0) {
        printk(KERN_INFO "lab_driver: cannot register the device\r");
    }

    proc_create("info", 0666, proc_root, &fops);
    printk(KERN_INFO "lab_driver: registered\r");
    return SUCCESS;
}

static void __exit lab_driver_exit(void)
{
    printk(KERN_INFO "lab_driver: exit()\r");
    proc_remove(proc_root);
    unregister_chrdev(240, "lab_driver");
}

module_init(lab_driver_init);
module_exit(lab_driver_exit);