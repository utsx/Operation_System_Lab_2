#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../driver_structs.h"


void help() {
    fprintf(stderr, "Usage: ./user <pid> process id\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        help();
    }
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments\n");
        return -1;
    }
    if (argc > 2) {
        fprintf(stderr, "Too many arguments\n");
        return -1;
    }
    char *endptr;
    errno = 0;
    int pid = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        help();
        perror("Invalid struct_id");
        return -1;
    }
    if (pid < 0) {
        fprintf(stderr, "Wrong pid\n");
        return -1;
    }
    int fd = open("/proc/lab_driver", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    struct answer answer = (struct answer) {
            .pid = pid,
            .inode = (struct user_inode) {
                    .i_ino = 0,
                    .i_mode = 0,
                    .i_flags = 0,
                    .i_size = 0,
                    .i_blocks = 0,
                    .i_uid = 0,
                    .i_gid = 0
            },
            .pci_dev = (struct user_pci_dev) {
                    .vendor = 0,
                    .device = 0,
                    .subsystem_vendor = 0,
                    .subsystem_device = 0
            }
    };
    if (write(fd, &answer, sizeof (struct answer)) < 0) {
        perror("write");
        return -1;
    }
    if (read(fd, &answer, sizeof(struct answer)) < 0) {
        perror("read");
        return -1;
    }
    if (answer.pid != -1) {
        printf("PCI_DEV and INODE for: %d\nPCI_DEV\nvendor: %d\ndevice: %d\nsubsystem_vendor: %d\nsubsystem_device: %d\nINODE"
               "\ni_ino: %lu\ni_mode: %d\ni_flags: %d\ni_size: %ld\ni_blocks: %ld\ni_uid: %d\ni_gid: %d\n",
               answer.pid, answer.pci_dev.vendor, answer.pci_dev.device, answer.pci_dev.subsystem_vendor, answer.pci_dev.subsystem_device,
               answer.inode.i_ino, answer.inode.i_mode, answer.inode.i_flags, answer.inode.i_size, answer.inode.i_blocks, answer.inode.i_uid, answer.inode.i_gid);
    } else {
        fprintf(stderr, "Can't find structs\n");
        return -1;
    }
    return 0;
}