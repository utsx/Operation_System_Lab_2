#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>

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
//static *pci_dev get_pci_dev();

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
//    size_t len = 0;
      struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
//    struct pci_dev *dev = NULL;
//    pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev);
//    printk(KERN_INFO "PCI_DEV \r", dev -> vendor);
    printk(KERN_INFO "lab_driver: read()\r");
    if (task == NULL) {
        printk(KERN_INFO "lab_driver: task is NULL\r");
        return SUCCESS;
    }

    return SUCCESS;
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

//static *pci_dev get_pci_dev(){
//    struct pci_dev *dev = NULL;
//    printk(KERN_INFO "lab_driver: get_pci_dev()\r");
//    dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev);
//    return dev;
//}


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