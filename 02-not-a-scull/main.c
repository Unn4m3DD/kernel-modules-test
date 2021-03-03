#include "main.h"

MODULE_AUTHOR("Andre Patacas");
MODULE_LICENSE("Dual BSD/GPL");
ssize_t not_a_scull_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos) {
  printk(KERN_ALERT "ssize_t not_a_scull_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos) {");
  if (mutex_lock_interruptible(&lock))
    return -ERESTARTSYS;
  if (*f_pos + count > DEVICE_MAX_SIZE)
    count = DEVICE_MAX_SIZE - *f_pos;
  int err = copy_from_user(file_contents + *f_pos, buf, count);
  if (err) {
    printk(KERN_ALERT "copy_from_user error");
    return err;
  }
  int second_number = 0;
  int i;
  for (i = 0; i < count; i++) {
    if (file_contents[i] == '+') {
      second_number = i + 1;
      file_contents[i] = 0;
      break;
    }
  }
  int i1, i2;
  err = kstrtoint(file_contents, 10, &i1);
  if (err) {
    printk(KERN_ALERT "kstrtoint 1");
    return err;
  }
  err = kstrtoint(&file_contents[second_number], 10, &i2);
  if (err) {
    printk(KERN_ALERT "kstrtoint 2");
    return err;
  }
  i2 += i1;
  file_contents_length = sprintf(file_contents, "%d", i2);
  mutex_unlock(&lock);
  return count;
}
ssize_t not_a_scull_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos) {
  printk(KERN_ALERT "ssize_t not_a_scull_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos) {");
  if (mutex_lock_interruptible(&lock))
    return -ERESTARTSYS;
  if (*f_pos + count > file_contents_length)
    count = file_contents_length - *f_pos;
  int err = copy_to_user(buf, file_contents + *f_pos, count);
  if (err) return err;
  mutex_unlock(&lock);
  file_contents_length = 0;
  return count;
}

int not_a_scull_release(struct inode* inode, struct file* filp) {
  printk(KERN_ALERT "int not_a_scull_release(struct inode* inode, struct file* filp) {");
  return 0;
}

int not_a_scull_open(struct inode* inode, struct file* filp) {
  printk(KERN_ALERT "int not_a_scull_open(struct inode* inode, struct file* filp) {");
  return 0;
}
static int not_a_scull_init(void) {
  printk(KERN_ALERT "static int not_a_scull_init(void) {");
  alloc_chrdev_region(&device_number, 0, 1, "not-a-scull");
  cdev_init(&char_device, &not_a_scull_fops);
  char_device.owner = THIS_MODULE;
  int err = cdev_add(&char_device, device_number, 1);
  if (err) {
    printk(KERN_ALERT "Error %d adding not_a_scull", err);
    unregister_chrdev(MAJOR(device_number), "not-a-scull");
    return -ENODEV;
  }

  device_class = class_create(THIS_MODULE, "not-a-scull");
  device = device_create(device_class, NULL, device_number, NULL, "not-a-scull");
  return 0;
}
static void not_a_scull_exit(void) {
  unregister_chrdev(MAJOR(device_number), "not-a-scull");
  cdev_del(&char_device);
  device_del(device);
  class_destroy(device_class);
  printk(KERN_ALERT "static void not_a_scull_exit(void) {");
}

module_init(not_a_scull_init);
module_exit(not_a_scull_exit);
