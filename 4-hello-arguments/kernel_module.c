
#include <linux/init.h>
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("erhoof");
MODULE_DESCRIPTION("A sample driver");

static char *own_string = "str";
module_param(own_string, charp, 0000); // 0000 is not NULL !!!!
MODULE_PARM_DESC(own_string, "Some string");

static int own_int = 0;
module_param(own_int, int,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // sysfs file permission
MODULE_PARM_DESC(own_int, "Some integer");

static int own_array[2] = {0, 1};
static int own_array_count = 0;
module_param_array(own_array, int, &own_array_count, 0000);
MODULE_PARM_DESC(own_array, "Some array of integers");

static int __init own_init_module(void) {
    pr_info("=============================");
    pr_info("own_module_init\n");
    pr_info("own_string, %s\n", own_string);
    pr_info("own_int, %d\n", own_int);
    for(int i = 0; i < own_array_count; ++i) {
        pr_info("own_array[%d], %d\n", i, own_array[i]);
    }

    /* A non 0 return means init_module failed; module can't be loaded. */
    return 0;
}

static void __exit own_module_exit(void) {
    pr_info("own_module_exit\n"); // \n is important!!
}

module_init(own_init_module);
module_exit(own_module_exit);
