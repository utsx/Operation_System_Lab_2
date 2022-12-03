//
// Created by dimma on 03.12.2022.
//

#ifndef OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H

#define SUCCESS 0
#define STRUCT_PCI_DEV 0
#define STRUCT_INODE 0
#define PROCFS_MAX_SIZE 1024

struct user_pci_dev{
    unsigned short vendor;
    unsigned short device;
    unsigned short subsystem_vendor;
    unsigned short subsystem_device;
};


struct user_inode{
    unsigned long i_ino;
    unsigned int i_mode;
    unsigned int i_nlink;
    unsigned int i_uid;
    unsigned int i_gid;
    unsigned long i_size;
    unsigned long i_atime;
    unsigned long i_mtime;
    unsigned long i_ctime;
    unsigned long i_blocks;
    unsigned long i_blksize;
    unsigned long i_flags;
    unsigned long i_generation;
    unsigned long i_file_acl;
    unsigned long i_dir_acl;
    unsigned long i_faddr;
    unsigned char i_frag;
    unsigned char i_fsize;
    unsigned short i_pad1;
    unsigned long i_reserved1;
    unsigned long i_reserved2;
};

#define OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H


#endif //OPERATION_SYSTEM_LAB_2_LAB_STRUCTS_H
