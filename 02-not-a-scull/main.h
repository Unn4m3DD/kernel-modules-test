#pragma once
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#define DEVICE_MAX_SIZE 100
int not_a_scull_release(struct inode* inode, struct file* filp);
int not_a_scull_open(struct inode* inode, struct file* filp);
ssize_t not_a_scull_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t not_a_scull_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);

dev_t device_number = 0;
struct class* device_class = NULL;
struct device* device = NULL;
struct cdev char_device;
char file_contents[DEVICE_MAX_SIZE];
int file_contents_length = 0;
struct mutex lock;
struct file_operations not_a_scull_fops = {
    .owner = THIS_MODULE,
    .read = not_a_scull_read,
    .write = not_a_scull_write,
    .open = not_a_scull_open,
    .release = not_a_scull_release,
};
