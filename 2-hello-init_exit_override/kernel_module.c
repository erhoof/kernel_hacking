/*
 * hello-1.c - The simplest kernel module.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

static int data __initdata = 3;

static int __init own_module_init(void) {
    pr_info("own_module_init, %d", data);

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

static void __exit own_module_exit(void) {
    pr_info("own_module_exit");
}

module_init(own_module_init);
module_exit(own_module_exit);

MODULE_LICENSE("GPL");