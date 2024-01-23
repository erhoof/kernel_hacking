
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("erhoof");
MODULE_DESCRIPTION("A sample driver");

int init_module(void) {
    pr_info("Hello, multiple files!\n");

    return 0;
}

