// Title: Kernel Source Tree IOCTL assignments
// URL: https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/Documentation/userspace-api/ioctl/ioctl-number.rst

/*
 * ioctl.c
 */
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct ioctl_arg
{
    unsigned int val;
};

/* Documentation/userspace-api/ioctl/ioctl-number.rst */
#define IOC_MAGIC '\x66'

/*
IOCTL number usually created by macro
    _IO, _IOR, _IOW, _IOWR (depending on the type) (r/w permissions)
    _IO - no argument
    _IOR - reading data from the driver
    _IOW - writing data to the driver
    _IOWR - bidirectional driver data transfer

Arguments: magic number, the command ID, data type that will be passed
    _IO has no data type

The magic number, an 8-bit integer selected to be specific to the device driver.
    type should be chosen so as not to conflict with other drivers that
    might be ``listening'' to the same file descriptor.
    (Inside the kernel, for example, the TCP and IP stacks use distinct numbers
    since an ioctl sent to a socket file descriptor might be examined by both stacks
tldr. is a unique number or character that will differentiate our set of ioctl
        calls from the other ioctl calls. some times the major number for
        the device is used here.
The command id, is a way for your dirver to understand what command
    is needed to be called.
Last parameter (the type) will allow the kernel to understand the size to be copied.
*/
#define IOCTL_VALSET _IOW(IOC_MAGIC, 0, struct ioctl_arg)
#define IOCTL_VALGET _IOR(IOC_MAGIC, 1, struct ioctl_arg)
#define IOCTL_VALGET_NUM _IOR(IOC_MAGIC, 2, int)
#define IOCTL_VALSET_NUM _IOW(IOC_MAGIC, 3, int)

// Dunno why IOCTL_VAL_MAXNR is needed, module compiles succesfully without it
// Google searches nothing for that
// guess it was left by this TLDP updated guide
#define IOCTL_VAL_MAXNR 3
#define DRIVER_NAME "ioctltest"

static unsigned int test_ioctl_major = 0; // ioctl major number (by MAJOR macro)
static unsigned int num_of_dev = 1; // count of device numbers to register
// ^ to reduce count of devices reserved for the MAJOR
static struct cdev test_ioctl_cdev; // represents character device
static int ioctl_num = 0;

struct test_ioctl_data
{
    unsigned char val;
    rwlock_t lock;
};

// cmd arg is IOCTL number
// it encodes the major device number, the type of ioctl,
//      the command, and the type of the parameter
static long test_ioctl_ioctl(struct file *filp, unsigned int cmd,
                             unsigned long arg)
{
    struct test_ioctl_data *ioctl_data = filp->private_data;
    int retval = 0;
    unsigned char val;
    struct ioctl_arg data;
    memset(&data, 0, sizeof(data));

    switch (cmd)
    {
    case IOCTL_VALSET:
        if (copy_from_user(&data, (int __user *)arg, sizeof(data)))
        {
            retval = -EFAULT;
            goto done;
        }

        pr_alert("IOCTL set val:%x .\n", data.val);
        write_lock(&ioctl_data->lock);
        ioctl_data->val = data.val;
        write_unlock(&ioctl_data->lock);
        break;

    case IOCTL_VALGET:
        read_lock(&ioctl_data->lock);
        val = ioctl_data->val;
        read_unlock(&ioctl_data->lock);
        data.val = val;

        if (copy_to_user((int __user *)arg, &data, sizeof(data)))
        {
            retval = -EFAULT;
            goto done;
        }

        break;

    case IOCTL_VALGET_NUM:
        retval = __put_user(ioctl_num, (int __user *)arg);
        break;

    case IOCTL_VALSET_NUM:
        ioctl_num = arg;
        break;

    default:
        retval = -ENOTTY;
    }

done:
    return retval;
}

static ssize_t test_ioctl_read(struct file *filp, char __user *buf,
                               size_t count, loff_t *f_pos)
{
    struct test_ioctl_data *ioctl_data = filp->private_data;
    unsigned char val;
    int retval;
    int i = 0;

    read_lock(&ioctl_data->lock);
    val = ioctl_data->val;
    read_unlock(&ioctl_data->lock);

    for (; i < count; i++)
    {
        if (copy_to_user(&buf[i], &val, 1))
        {
            retval = -EFAULT;
            goto out;
        }
    }

    retval = count;
out:
    return retval;
}

static int test_ioctl_close(struct inode *inode, struct file *filp)
{
    pr_alert("%s call.\n", __func__);

    if (filp->private_data)
    {
        kfree(filp->private_data);
        filp->private_data = NULL;
    }

    return 0;
}

static int test_ioctl_open(struct inode *inode, struct file *filp)
{
    struct test_ioctl_data *ioctl_data;

    pr_alert("%s call.\n", __func__);
    ioctl_data = kmalloc(sizeof(struct test_ioctl_data), GFP_KERNEL);

    if (ioctl_data == NULL)
        return -ENOMEM;

    rwlock_init(&ioctl_data->lock);
    ioctl_data->val = 0xFF;
    filp->private_data = ioctl_data;

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = test_ioctl_open,
    .release = test_ioctl_close,
    .read = test_ioctl_read,
    .unlocked_ioctl = test_ioctl_ioctl,
};

/*
register_chrdev_region uses count of requested device numbers (num_of_dev)
register_chrdev does not
also register_chrdev* does not create device file in advance, so you don't
know major number until it is used

With allocation (alloc_chrdev_region), you tell the kernel how many
device numbers you need (the starting minor number and count)
and it will find a starting major number
for you, if one is available, of course.
*/
static int __init ioctl_init(void)
{
    dev_t dev;
    int alloc_ret = -1;
    int cdev_ret = -1;
    // 0 is first minor, num_of dev is the count of the mirrors
    alloc_ret = alloc_chrdev_region(&dev, 0, num_of_dev, DRIVER_NAME);

    if (alloc_ret)
        goto error;

    test_ioctl_major = MAJOR(dev);
    cdev_init(&test_ioctl_cdev, &fops);
    cdev_ret = cdev_add(&test_ioctl_cdev, dev, num_of_dev);

    if (cdev_ret)
        goto error;

    pr_alert("%s driver(major: %d) installed.\n", DRIVER_NAME,
             test_ioctl_major);
    return 0;
error:
    if (cdev_ret == 0)
        cdev_del(&test_ioctl_cdev);
    if (alloc_ret == 0)
        unregister_chrdev_region(dev, num_of_dev);
    return -1;
}

static void __exit ioctl_exit(void)
{
    dev_t dev = MKDEV(test_ioctl_major, 0);

    cdev_del(&test_ioctl_cdev);
    unregister_chrdev_region(dev, num_of_dev);
    pr_alert("%s driver removed.\n", DRIVER_NAME);
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is test_ioctl module");
