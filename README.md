# Kernel Modules
This repository contains some kernel modules developed based on the Linux Device Drivers 3rd edition  
Documentation for what the modules do and their implementations are inside the folders  
As this modules are for learning purposes they are all called "main" as it is easier to have a single environment for all module compilation without creating a lot of garbage from make.  

To compile a module use
```sh
$ ./build.sh <directory_name>
```
To install it 
```
# make <install|remove>
```

There is also a utility to run ioctl commands, to install it 
```
cd ioctl
make install
cd ..
```
To run it
```
# ioctl <file-path> <cmd> [arg1] [arg2] ...
```