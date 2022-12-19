//
// Created by dimma on 03.12.2022.
//

#include <linux/types.h>
#ifndef OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H

#define SUCCESS 0
#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "lab_driver"

struct user_pci_dev{
    unsigned short vendor;
    unsigned short device;
    unsigned short subsystem_vendor;
    unsigned short subsystem_device;
};

struct user_inode{
    unsigned long i_ino;
    unsigned int i_mode;
    unsigned int i_flags;
    loff_t i_size;
    blkcnt_t i_blocks;
    uid_t i_uid;
    uid_t i_gid;
};


struct answer{
    int pid;
    struct user_inode inode;
    struct user_pci_dev pci_dev;
};

#define OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H


#endif //OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H
