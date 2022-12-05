//
// Created by utsx on 05.12.22.
//

#ifndef OPERATION_SYSTEM_LAB_2_USER_H
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

#define OPERATION_SYSTEM_LAB_2_USER_H

#endif //OPERATION_SYSTEM_LAB_2_USER_H