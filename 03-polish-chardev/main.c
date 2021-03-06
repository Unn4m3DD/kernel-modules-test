#include "main.h"

char buffer[DEVICE_MAX_SIZE];

typedef struct {
  int data[STACK_MAX_SIZE];
  int size;
} my_stack_t;

my_stack_t stack = {
    .size = 0};

MODULE_AUTHOR("Andre Patacas");
MODULE_LICENSE("Dual BSD/GPL");

int stack_to_string(my_stack_t* stack, char* buffer, int max_size) {
  int i, result;
  int written_size = 0;
  for (i = 0; i < stack->size; i++) {
    if (stack->data[i] == -1)
      result = snprintf(&buffer[written_size], max_size - written_size, "+ ");
    else if (stack->data[i] == -2)
      result = snprintf(&buffer[written_size], max_size - written_size, "- ");
    else if (stack->data[i] == -3)
      result = snprintf(&buffer[written_size], max_size - written_size, "* ");
    else if (stack->data[i] == -4)
      result = snprintf(&buffer[written_size], max_size - written_size, "/ ");
    else
      result = snprintf(&buffer[written_size], max_size - written_size, "%d ", stack->data[i]);
    if (result < 0) {
      printk(KERN_ALERT "scnprinf error %d\n", result);
      return result;
    }
    written_size += result;
  }
  buffer[written_size] = '\n';
  buffer[written_size + 1] = '\0';
  return written_size + (written_size > 0 ? 2 : 0);
}

int stack_push(my_stack_t* stack, int item) {
  if (stack->size == STACK_MAX_SIZE) {
    printk(KERN_ALERT "stack_push failed: max size reached\n");
    return -ENOSPC;
  }
  stack->data[stack->size++] = item;
  return stack->size;
}

int stack_pop(my_stack_t* stack, int* item) {
  if (stack->size == 0) {
    printk(KERN_ALERT "stack_pop failed: empty stack\n");
    return -ENODATA;
  }
  stack->size--;
  *item = stack->data[stack->size];
  return stack->size;
}

int parse_operator(my_stack_t* stack, int operator) {
  int item1;
  stack_pop(stack, &item1);
  if (item1 < 0)
    item1 = parse_operator(stack, item1);

  int item2;
  stack_pop(stack, &item2);
  if (item2 < 0)
    item2 = parse_operator(stack, item2);
  if (operator== - 1) return item1 + item2;
  if (operator== - 2) return item1 - item2;
  if (operator== - 3) return item1 * item2;
  if (operator== - 4) return item1 / item2;
  return -5;
}

int stack_compute(my_stack_t* stack) {
  int first_op;
  stack_pop(stack, &first_op);
  stack_push(stack, parse_operator(stack, first_op));
  return 0;
}

int push_index(int i) {
  int number;
  int err;
  if (buffer[i] == '+')
    err = stack_push(&stack, -1);
  else if (buffer[i] == '-')
    err = stack_push(&stack, -2);
  else if (buffer[i] == '*')
    err = stack_push(&stack, -3);
  else if (buffer[i] == '/')
    err = stack_push(&stack, -4);
  else {
    err = kstrtoint(&buffer[i], 10, &number);
    if (err) {
      printk(KERN_ALERT "kstrtoint failed for %s with error %d", &buffer[i], err);
      return err;
    }
    err = stack_push(&stack, number);
  }
  if (err < 0)
    printk(KERN_ALERT "push_index failed: stack max size reached\n");

  return err;
}
long polish_reverse_chardev_ioctl(struct file* filp, unsigned int cmd, unsigned long arg) {
  printk(KERN_ALERT " polish_reverse_chardev_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)");
  if (mutex_lock_interruptible(&lock))
    return -ERESTARTSYS;
  int result = 0;
  if (cmd == 0)
    stack.size = 0;
  else
    result = stack_compute(&stack);
  mutex_unlock(&lock);
  return result;
}

ssize_t polish_reverse_chardev_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos) {
  printk(KERN_ALERT "ssize_t polish_reverse_chardev_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)\n");
  if (mutex_lock_interruptible(&lock))
    return -ERESTARTSYS;
  ssize_t result = count;
  if (result >= DEVICE_MAX_SIZE)
    result = DEVICE_MAX_SIZE - 1;
  int err = copy_from_user(buffer, buf, result);
  if (err < 0) {
    printk(KERN_ALERT "copy_from_user error\n");
    result = err;
    goto write_cleanup;
  }
  buffer[result] = 0;
  stack.size = 0;
  int i;
  for (i = result - 1; i >= 0; i--)
    if (buffer[i] == ' ') {
      buffer[i] = 0;
      err = push_index(i + 1);
      if (err < 0) {
        result = err;
        goto write_cleanup;
      }
    }
  err = push_index(0);
  if (err < 0) {
    result = err;
    goto write_cleanup;
  }

write_cleanup:
  mutex_unlock(&lock);
  return result;
}
ssize_t polish_reverse_chardev_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos) {
  printk(KERN_ALERT "ssize_t polish_reverse_chardev_read(struct file* filp, char __user* buf, %ld, %lld)\n",
         count, *f_pos);
  if (mutex_lock_interruptible(&lock))
    return -ERESTARTSYS;

  int result = count;
  int string_size = stack_to_string(&stack, buffer, DEVICE_MAX_SIZE);
  if (string_size < 0) {
    result = string_size;
    goto read_cleanup;
  }
  if (*f_pos + result > string_size)
    result = string_size - *f_pos;
  int err = copy_to_user(buf, buffer + *f_pos, result);
  if (err < 0) {
    printk(KERN_ALERT "error on copy_to_user\n");
    result = err;
    goto read_cleanup;
  }

read_cleanup:
  mutex_unlock(&lock);
  if (*f_pos == string_size) return 0;
  if (result > 0)
    *f_pos += string_size;
  return result;
}

int polish_reverse_chardev_release(struct inode* inode, struct file* filp) {
  printk(KERN_ALERT "int polish_reverse_chardev_release(struct inode* inode, struct file* filp)\n");
  return 0;
}

int polish_reverse_chardev_open(struct inode* inode, struct file* filp) {
  printk(KERN_ALERT "int polish_reverse_chardev_open(struct inode* inode, struct file* filp)\n");
  return 0;
}

static int polish_reverse_chardev_init(void) {
  printk(KERN_ALERT "static int polish_reverse_chardev_init(void)\n");
  alloc_chrdev_region(&device_number, 0, 1, "reverse-polish-chardev");
  cdev_init(&char_device, &polish_reverse_chardev_fops);
  char_device.owner = THIS_MODULE;
  int err = cdev_add(&char_device, device_number, 1);
  if (err) {
    printk(KERN_ALERT "Error %d adding polish_reverse_chardev", err);
    unregister_chrdev(MAJOR(device_number), "reverse-polish-chardev");
    return -ENODEV;
  }

  device_class = class_create(THIS_MODULE, "reverse-polish-chardev");
  device = device_create(device_class, NULL, device_number, NULL, "reverse-polish-chardev");
  return 0;
}

static void polish_reverse_chardev_exit(void) {
  unregister_chrdev(MAJOR(device_number), "reverse-polish-chardev");
  cdev_del(&char_device);
  device_del(device);
  class_destroy(device_class);
  printk(KERN_ALERT "static void polish_reverse_chardev_exit(void)\n");
}

module_init(polish_reverse_chardev_init);
module_exit(polish_reverse_chardev_exit);
