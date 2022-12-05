//
// Created by utsx on 05.12.22.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "user.h"


void help(){
    fprintf(stderr, "Usage: ./user <struct_id> <pid>\n0 - pci_dev, 1 - inode\nprocess id\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        help();
    }
    if(argc < 3){
        fprintf(stderr, "Not enough arguments\n");
        return -1;
    }
    if(argc > 3){
        fprintf(stderr, "Too many arguments\n");
        return -1;
    }
    char *endptr;
    errno = 0;
    int struct_id = strtol(argv[1], &endptr, 10);
    if(errno != 0 || *endptr != '\0'){
        help();
        perror("Invalid struct_id");
        return -1;
    }
    errno = 0;
    int pid = strtol(argv[2], &endptr, 10);
    if(errno != 0 || *endptr != '\0'){
        help();
        perror("PID in a wrong format");
        return -1;
    }
    if(struct_id != 0 && struct_id != 1){
        fprintf(stderr, "Wrong struct id\n");
        return -1;
    }
   if(pid < 0){
       fprintf(stderr, "Wrong pid\n");
         return -1;
   }
       int fd = open("/proc/lab_driver", O_RDWR);
        if(fd < 0){
            perror("open");
            return -1;
        }
        char buf[100];
        int num_of_args = sprintf(buf, "%d %d", struct_id, pid);
        if(num_of_args < 0){
            perror("sprintf");
            return -1;
        }
        if(write(fd, buf, num_of_args) < 0){
            perror("write");
            return -1;
        }
        if(struct_id == 0){
            struct user_pci_dev user_pci_dev;
            if(read(fd, &user_pci_dev, sizeof(struct user_pci_dev)) < 0){
                perror("read");
                return -1;
            }
            printf("vendor: %d\ndevice: %d\nsubsystem_vendor: %d\nsubsystem_device: %d\n",
                user_pci_dev.vendor, user_pci_dev.device, user_pci_dev.subsystem_vendor, user_pci_dev.subsystem_device);
        }
        if(struct_id == 1){
            struct user_inode user_inode;
            if(read(fd, &user_inode, sizeof(struct user_inode)) < 0){
                perror("read");
                return -1;
            }
            printf("i_ino: %d\ni_mode: %d\n", user_inode.i_ino, user_inode.i_mode);
        }
        return 0;


}