#ifndef KE2DBG_SYSCALLS_H
 #define KE2DBG_SYSCALLS_H

#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

#include <linux/dirent.h>

typedef void (*sighandler_t)(int);

static void **ke2dbg_syscalltable;

asmlinkage ssize_t sys_read(int, void *, size_t);
asmlinkage ssize_t sys_write(int, const void *, size_t);
asmlinkage int sys_open(const char *, int, int);
asmlinkage int sys_close(int);
asmlinkage sighandler_t sys_signal(int, sighandler_t);
asmlinkage int sys_execve(struct pt_regs);
asmlinkage int sys_fsync(int);
asmlinkage int sys_readdir(unsigned int, struct dirent *, unsigned int);
asmlinkage int sys_fork(struct pt_regs);
asmlinkage long sys_exit(int);

asmlinkage int (*ke2dbg_sys_read)(int, void *, size_t);
asmlinkage int (*ke2dbg_sys_write)(int, const void *, size_t);
asmlinkage int (*ke2dbg_sys_open)(const char *, int, int);
asmlinkage int (*ke2dbg_sys_close)(int);
asmlinkage sighandler_t (*ke2dbg_sys_signal)(int, sighandler_t);
asmlinkage int (*ke2dbg_sys_execve)(struct pt_regs);
asmlinkage int (*ke2dbg_sys_fsync)(int);
asmlinkage int (*ke2dbg_sys_readdir)(unsigned int, struct dirent *, unsigned int);
asmlinkage int (*ke2dbg_sys_fork)(struct pt_regs);
asmlinkage long (*ke2dbg_sys_exit)(int);

#endif
