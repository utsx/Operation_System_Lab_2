#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include "lab_structs.h"

#define SUCCESS 0
#define BUF_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitrii Aleksandrov");

static int pid = 1;
static int struct_id = 1;
static unsigned long addr = 0;

static struct proc_dir_entry *proc_root;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

static int lab_driver_open(struct inode *inode, struct file *file);
static int lab_driver_release(struct inode *inode, struct file *file);
static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static struct user_pci_dev *get_pci_dev(struct pci_dev *dev);
static struct write_pci_dev write_pci_dev(struct user_pci_dev *dev);

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
    char buffer[BUF_SIZE];
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    printk(KERN_INFO "lab_driver: read()\r");
    if (task == NULL) {
        printk(KERN_INFO "lab_driver: task is NULL\r");
        return SUCCESS;
    }

    return SUCCESS;
}

struct user_pci_dev *get_pci_dev(struct pci_dev *dev)
{
    struct user_pci_dev *user_dev = kmalloc(sizeof(struct user_pci_dev), GFP_KERNEL);
    user_dev->vendor = dev->vendor;
    user_dev->device = dev->device;
    user_dev->subsystem_vendor = dev->subsystem_vendor;
    user_dev->subsystem_device = dev->subsystem_device;
    return user_dev;
}

static struct write_pci_dev write_pci_dev(struct user_pci_dev *dev) {
    char buffer[BUF_SIZE];
    size_t len = 0;
    len += sprintf(buffer + len, "vendor: %d", dev->vendor);
    len += sprintf(buffer + len, "device: %d", dev->device);
    len += sprintf(buffer + len, "subsystem_vendor: %d", dev->subsystem_vendor);
    len += sprintf(buffer + len, "subsystem_device: %d", dev->subsystem_device);
    if (copy_to_user(buf, buffer, len))
        return -EFAULT;
    return len;
}

static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char buffer[1024];
    size_t len = 0;
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    printk(KERN_INFO "lab_driver: write()\r");
    if (task == NULL) {
        printk(KERN_INFO "lab_driver: task is NULL\r");
        return SUCCESS;
    }
    return SUCCESS;
}




static int __init lab_driver_init(void)
{
    printk(KERN_INFO "lab_driver: init()\r");
    proc_root = proc_mkdir("lab_driver", NULL);
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