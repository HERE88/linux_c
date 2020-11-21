#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/device.h>
#include <linux/cdev.h>

static unsigned int major; /* major number for device */
static struct class *dummy_class;

struct test_st111
{
	struct cdev dummy_cdev1;
	int aaa;
	
};

struct test_st111 test = {
	.aaa =999,
};

static struct cdev dummy_cdev;


int dummy_open(struct inode * inode, struct file * filp)
{
	struct test_st111 * p3 = container_of(inode->i_cdev, struct test_st111, dummy_cdev1);
	p3->aaa = 888;
	
	filp->private_data = p3;
    pr_info("Someone tried to open me\n");
    return 0;
}

int dummy_release(struct inode * inode, struct file * filp)
{
    pr_info("Someone closed me\n");
    return 0;
}

ssize_t dummy_read (struct file *filp, char __user * buf, size_t count,
                                loff_t * offset)
{
	
	//pr_info("Test begin---------------\n");
	struct test_st111 * p2 = (struct test_st111 *)(filp->private_data);
	pr_info("p2->aaa=%d\n", p2->aaa);
	
	pr_info("Test end-----------------\n");
    pr_info("Nothing to read guy\n");
    return 0;
}


ssize_t dummy_write(struct file * filp, const char __user * buf, size_t count,
                                loff_t * offset)
{
    pr_info("Can't accept any data guy\n");
    return count;
}

struct file_operations dummy_fops = {
    open:       dummy_open,
    release:    dummy_release,
    read:       dummy_read,
    write:      dummy_write,
};

static int __init dummy_char_init_module(void)
{
    struct device *dummy_device;
    int error;
    dev_t devt = 0;

    /* Get a range of minor numbers (starting with 0) to work with */
    error = alloc_chrdev_region(&devt, 0, 1, "dummy_char");
    if (error < 0) {
        pr_err("Can't get major number\n");
        return error;
    }
    major = MAJOR(devt);
    pr_info("dummy_char major number = %d\n",major);

    /* Create device class, visible in /sys/class */
    dummy_class = class_create(THIS_MODULE, "dummy_char_class");
    if (IS_ERR(dummy_class)) {
        pr_err("Error creating dummy char class.\n");
        unregister_chrdev_region(MKDEV(major, 0), 1);
        return PTR_ERR(dummy_class);
    }

    /* Initialize the char device and tie a file_operations to it */
    cdev_init(&test.dummy_cdev1, &dummy_fops);
    test.dummy_cdev1.owner = THIS_MODULE;
    /* Now make the device live for the users to access */
    cdev_add(&test.dummy_cdev1, devt, 1);

    dummy_device = device_create(dummy_class,
                                NULL,   /* no parent device */
                                devt,    /* associated dev_t */
                                NULL,   /* no additional data */
                                "wht1111");  /* device name */

    if (IS_ERR(dummy_device)) {
        pr_err("Error creating dummy char device.\n");
        class_destroy(dummy_class);
        unregister_chrdev_region(devt, 1);
        return -1;
    }

    pr_info("dummy char module loaded\n");
    return 0;
}

static void __exit dummy_char_cleanup_module(void)
{
    unregister_chrdev_region(MKDEV(major, 0), 1);
    device_destroy(dummy_class, MKDEV(major, 0));
    cdev_del(&test.dummy_cdev1);
    class_destroy(dummy_class);

    pr_info("dummy char module Unloaded\n");
}

module_init(dummy_char_init_module);
module_exit(dummy_char_cleanup_module);

MODULE_AUTHOR("John Madieu <john.madieu@gmail.com>");
MODULE_DESCRIPTION("Dummy character driver");
MODULE_LICENSE("GPL");
