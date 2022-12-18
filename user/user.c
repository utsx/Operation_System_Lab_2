#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../driver_structs.h"


void help() {
    fprintf(stderr, "Usage: ./user <struct_id> <pid>\n0 - pci_dev, 1 - inode\nprocess id\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        help();
    }
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        return -1;
    }
    if (argc > 3) {
        fprintf(stderr, "Too many arguments\n");
        return -1;
    }
    char *endptr;
    errno = 0;
    int struct_id = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        help();
        perror("Invalid struct_id");
        return -1;
    }
    errno = 0;
    int pid = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        help();
        perror("PID in a wrong format");
        return -1;
    }
    if (struct_id != 0 && struct_id != 1) {
        fprintf(stderr, "Wrong struct id\n");
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
    char buf[100];
    int num_of_args = sprintf(buf, "%d %d", struct_id, pid);
    if (num_of_args < 0) {
        perror("sprintf");
        return -1;
    }
    if (write(fd, buf, num_of_args) < 0) {
        perror("write");
        return -1;
    }
    struct answer answer;
    if (read(fd, &answer, sizeof(struct answer)) < 0) {
        perror("read");
        return -1;
    }
    if (answer.struct_id == 0) {
        printf("PCI_DEV for: %d\nvendor: %d\ndevice: %d\nsubsystem_vendor: %d\nsubsystem_device: %d\n",
               answer.pid, answer.pci_dev.vendor, answer.pci_dev.device, answer.pci_dev.subsystem_vendor,
               answer.pci_dev.subsystem_device);
    } else if (answer.struct_id == 1) {
        printf("INODE for: %d\ni_ino: %ld\ni_mode: %d\ni_flags: %d\ni_size: %ld\ni_blocks: %ld\n",
               answer.pid, answer.inode.i_ino, answer.inode.i_mode, answer.inode.i_flags,
               answer.inode.i_size, answer.inode.i_blocks);
    } else {
        fprintf(stderr, "There is no such this PID: %d\n", answer.pid);
        return -1;
    }
    return 0;
}