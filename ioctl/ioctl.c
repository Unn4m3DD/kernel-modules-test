#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
int main(int argc, char** argv) {
  if (argc <= 2) {
    printf("Usage: ioctl <file-path> <cmd> [arg1] [arg2] ...\n");
    return -1;
  }
  int fd = open(argv[1], O_RDWR);
  int ret = ioctl(fd, atoi(argv[2]), (unsigned long int)argc < 4 ? NULL : &argv[3]);
  if (ret == -1) {
    perror("ioctl");
    printf("Please check if the file exists and you have the right permissions to it\n");
  }
  close(fd);
}