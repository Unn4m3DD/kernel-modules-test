obj-m += main.o
ccflags-y := -Wno-declaration-after-statement
.PHONN: install remove

build:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

install:
		(! (lsmod | grep main)) || sudo rmmod main
		sudo insmod build/main.ko

remove:
		sudo rmmod main

install_ioctl:
