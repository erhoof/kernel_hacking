
#include <linux/init.h>
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("erhoof");
MODULE_DESCRIPTION("A sample driver");

static int __init own_init_module(void) {
    pr_info("own_module_init");

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

static void __exit own_module_exit(void) {
    pr_info("own_module_exit");
}

module_init(own_init_module);
module_exit(own_module_exit);
