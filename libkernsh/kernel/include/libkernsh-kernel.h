#ifndef LIBKERNSH_KERNEL_H
 #define LIBKERNSH_KERNEL_H

#include "libkernsh-virtm.h"

#if defined(__linux__)
#include <linux/linkage.h>
#endif

/* page.c */
struct page *kernsh_get_page_from_pid(int pid, unsigned long);

/* virtm.c */
int kernsh_dump_elf_pid(int, const char *);
asmlinkage int kernsh_read_virtm(int, unsigned long, char *, int, int);
asmlinkage int kernsh_write_virtm(int, unsigned long, const char *, int, int);

/* mem.c */
asmlinkage int kernsh_read_mem(unsigned long, char *, int, int);
asmlinkage int kernsh_write_mem(unsigned long, const char *, int, int);

/* misc.c */
int kernsh_atoi(const char *);
int kernsh_view_vmaps(int);
asmlinkage int kernsh_task_pid(int, char *, int, int);
int valid_phys_addr_range(unsigned long, int);

#endif /* __LIBKERNSH_KERNEL_H_ */
