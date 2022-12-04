//
// Created by dimma on 03.12.2022.
//

#ifndef OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H

#define SUCCESS 0
#define STRUCT_PCI_DEV 0
#define STRUCT_INODE 1
#define PROCFS_MAX_SIZE 1024

struct user_pci_dev{
    unsigned short vendor;
    unsigned short device;
    unsigned short subsystem_vendor;
    unsigned short subsystem_device;
};

#define OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H


#endif //OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H
