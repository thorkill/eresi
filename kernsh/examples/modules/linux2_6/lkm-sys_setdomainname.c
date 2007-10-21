#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int asmlinkage new_sys_setdomainname(const char *name, size_t len)
{
	printk("NEW SYS DOMAINNAME\n");

	return old_sys_setdomainname(name, len);
}

static int testreladd_init(void)
{
	printk(KERN_ALERT "TEST REL ADD INIT\n");
	return 0;
}

static void testreladd_exit(void)
{
	printk(KERN_ALERT "TEST REL ADD EXIT\n");
}


module_init(testreladd_init);
module_exit(testreladd_exit);
