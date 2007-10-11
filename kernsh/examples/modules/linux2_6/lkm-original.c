#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int original_init(void)
{
	printk(KERN_ALERT "ORIGINAL INIT\n");
	return 0;
}

static void original_exit(void)
{
	printk(KERN_ALERT "ORIGINAL END\n");
}


module_init(original_init);
module_exit(original_exit);
