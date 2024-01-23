/*
 * hello-1.c - The simplest kernel module.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

int init_module(void)
{
    pr_info("Wow, @erhoof finally got some Linux in his hands!\n");

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

void cleanup_module(void)
{
    pr_info("Wow, he is trying to kill me =(\n");
}

MODULE_LICENSE("GPL");