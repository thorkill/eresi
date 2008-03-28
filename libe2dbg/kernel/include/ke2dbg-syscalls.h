#ifndef KE2DBG_SYSCALLS_H
 #define KE2DBG_SYSCALLS_H

#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

#include <linux/dirent.h>
#include <linux/resource.h>
#include <linux/posix_types.h>

typedef void (*sighandler_t)(int);

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
asmlinkage long sys_rename(const char *, const char *);
asmlinkage long sys_munmap(unsigned long, size_t);
asmlinkage long sys_mmap2(unsigned long, unsigned long, unsigned long, 
			  unsigned long, unsigned long , unsigned long);
asmlinkage long sys_unlink(const char *);
asmlinkage long sys_mprotect(unsigned long, size_t, unsigned long);
asmlinkage long sys_wait4(pid_t, int *, int, struct rusage *);
asmlinkage long sys_waitpid(pid_t, int *, int);
asmlinkage long sys_access(const char *, int);
asmlinkage long sys_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
asmlinkage long sys_dup2(unsigned int, unsigned int);
asmlinkage off_t sys_lseek(unsigned int, off_t, unsigned int);
asmlinkage long sys_mkdir(const char *, int);
asmlinkage long sys_nanosleep(struct timespec *, struct timespec *);

#endif
