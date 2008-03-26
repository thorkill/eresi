#ifndef LIBKERNSH_KERNEL_H
 #define LIBKERNSH_KERNEL_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/threads.h>
#include <linux/string.h>
#include <linux/bio.h>

#include "libkernsh-virtm.h"

/* page.c */
struct page *kernsh_get_page_from_task(struct task_struct *, unsigned long);

/* virtm.c */
int kernsh_dump_elf_pid(pid_t, const char *);
asmlinkage int kernsh_read_virtm(pid_t, unsigned long, char *, int, int);
asmlinkage int kernsh_write_virtm(pid_t, unsigned long, const char *, int, int);

/* mem.c */
asmlinkage int kernsh_read_mem(unsigned long, char *, int, int);
asmlinkage int kernsh_write_mem(unsigned long, const char *, int, int);

/* misc.c */
int kernsh_atoi(const char *);
int kernsh_view_vmaps(pid_t);
asmlinkage int kernsh_task_pid(pid_t, char *, int, int);
int valid_phys_addr_range(unsigned long, size_t);

#endif /* __LIBKERNSH_KERNEL_H_ */
