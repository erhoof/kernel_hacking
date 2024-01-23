
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("erhoof");
MODULE_DESCRIPTION("A sample driver");

void cleanup_module(void) {
    pr_info("own_module_exit\n"); // \n is important!!
}
