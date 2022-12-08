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

static int pid = 0;
static int struct_id = 0;

static char procfs_buffer[PROCFS_MAX_SIZE];

static unsigned long procfs_buffer_size = 0;

static struct proc_dir_entry *proc_root;

static int __init lab_driver_init(void);
static void __exit lab_driver_exit(void);

//module parameters

static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

static const struct proc_ops proc_ops = {
    .proc_read = lab_driver_read,
    .proc_write = lab_driver_write
};


static ssize_t lab_driver_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct answer answer;
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if(task == NULL){
        printk(KERN_INFO "lab_driver: task is null");
        answer = (struct answer){.pid = pid,
                .struct_id = -1};
    }
    else if(task != NULL){
        struct mm_struct *mm = task->mm;
        struct vm_area_struct *vma = mm->mmap;
        struct inode *inode = vma->vm_file->f_inode;
        struct pci_dev *dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
        answer = (struct answer) {
            .pid = pid,
            .struct_id = struct_id,
            .inode = {
                    .i_ino = inode->i_ino,
                    .i_mode = inode->i_mode,
                    .i_flags = inode->i_flags,
                    .i_size = inode->i_size,
                    .i_blocks = inode->i_blocks
                            },
            .pci_dev = {
                    .vendor = dev->vendor,
                    .device = dev->device,
                    .subsystem_vendor = dev->subsystem_vendor,
                    .subsystem_device = dev->subsystem_device
            }
        };
    }
    if(copy_to_user(buf, &answer, sizeof(struct answer)) != 0){
        printk(KERN_INFO "lab_driver: copy_to_user failed");
        return -1;
    }
    return SUCCESS;
}


static ssize_t lab_driver_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int num_of_args, read_struct_id, read_pid, c;
    procfs_buffer_size = count;
    if(*ppos > 0 || count > PROCFS_MAX_SIZE){
        printk(KERN_INFO "lab_driver: write failed");
        return -EFAULT;
    }
    if(copy_from_user(procfs_buffer, buf, count) != 0){
        printk(KERN_INFO "lab_driver: copy_from_user failed");
        return -EFAULT;
    }
    num_of_args = sscanf(buf, "%d %d", &read_struct_id, &read_pid);
    if(num_of_args != 2){
        printk(KERN_INFO "lab_driver: wrong number of arguments");
        return -1;
    }
    if(read_struct_id != STRUCT_PCI_DEV && read_struct_id != STRUCT_INODE){
        printk(KERN_INFO "lab_driver: wrong struct id");
        return -1;
    }
    pid = read_pid;
    struct_id = read_struct_id;
    c = strlen(procfs_buffer);
    *ppos = c;
    printk(KERN_INFO "lab_driver: pid = %d, struct_id = %d", pid, struct_id);
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