#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern int init_module(void);

int inject(void)
{
        printk(KERN_ALERT "INJECT !!!\n");
        init_module();
        return 0;
}
