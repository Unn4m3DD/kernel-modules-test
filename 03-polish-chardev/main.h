#pragma once
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define DEVICE_MAX_SIZE 1000
#define STACK_MAX_SIZE 100
int polish_reverse_chardev_release(struct inode* inode, struct file* filp);
int polish_reverse_chardev_open(struct inode* inode, struct file* filp);
ssize_t polish_reverse_chardev_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t polish_reverse_chardev_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);
long polish_reverse_chardev_ioctl(struct file* filp, unsigned int cmd, unsigned long arg);

dev_t device_number = 0;
struct class* device_class = NULL;
struct device* device = NULL;
struct cdev char_device;
struct mutex lock;
struct file_operations polish_reverse_chardev_fops = {
    .owner = THIS_MODULE,
    .read = polish_reverse_chardev_read,
    .write = polish_reverse_chardev_write,
    .open = polish_reverse_chardev_open,
    .release = polish_reverse_chardev_release,
    .unlocked_ioctl = polish_reverse_chardev_ioctl};
