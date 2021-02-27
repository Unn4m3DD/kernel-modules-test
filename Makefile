obj-m += hello.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
		mv hello.ko build/hello.c
		make clean
		mv build/hello.c build/hello.ko

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

install:
		sudo insmod build/hello.ko

remove:
		sudo rmmod build/hello.ko