#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//
// Created by utsx on 02.12.22.
//
int main(int argc, char *argv[]){

    char outbuf[4096];

    int fd = open("/proc/lab_driver/info", O_RDWR);
    write(fd, "hello", 5);
    lseek(fd, 0, SEEK_SET);
    read(fd, outbuf, 5);
    puts(outbuf);
}