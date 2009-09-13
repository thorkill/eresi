/**
* @file libe2dbg/kernel/kentry.c
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <stdarg.h>

extern int ke2dbg_init(void);
extern void ke2dbg_cleanup(void);

/**
 *
 */
int init_module()
{
  MODULE_LICENSE("GPL");
  ke2dbg_init();
  return 0;
}

/**
 *
 */
void cleanup_module()
{
  ke2dbg_cleanup();
}
