//
// Created by dimma on 03.12.2022.
//

#ifndef OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H

#define SUCCESS 0
#define STRUCT_PCI_DEV 0
#define STRUCT_INODE 1
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
};

#define OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H


#endif //OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H
