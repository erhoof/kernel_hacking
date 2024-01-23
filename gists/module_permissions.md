# Example. Setting permissions for module created devices

It is important to note that the standard roles of read and write are reversed in the kernel. Read functions are used for output, whereas write functions are used for input. The reason for that is that read and write refer to the user’s point of view — if a process reads something from the kernel, then the kernel needs to output it, and if a process writes something to the kernel, then the kernel receives it as input.

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
