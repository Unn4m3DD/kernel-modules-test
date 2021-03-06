#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
int main() {
  int fd = open("/dev/reverse-polish-chardev", O_RDWR);
  int ret = ioctl(fd, 1);
  if (ret == -1) {
    perror("ioctl");
  }
  close(fd);
}