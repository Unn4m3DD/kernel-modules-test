This module creates a char device named polish-chardev on /dev
You can echo a string like "([0-9]+|[+/*-])\s" meaning an expression in prefix notation with items separated by space 
After that issue the ioctl with command != 0 on the device and it will compute the expression.
To clear the buffer issue ioctl with command == 0 