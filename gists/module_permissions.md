# Example. Setting permissions for module created devices

``` cpp
#include <linux/miscdevice.h>

static struct miscdevice my_module_cnf = {
    /*some other fields*/,
    .mode = 0666,
};

static int __init my_module_init(void)
{
    int ret = misc_register(&my_module_cnf);
    if (ret < 0) {
        printk(KERN_ERR "Unable to register my module");
        return ret;
    }

    return 0;
}

static void __exit my_module_exit(void)
{
    misc_deregister(&my_module_cnf);
}

module_init(my_module_init);
module_exit(my_module_exit);
```
