/** 
 ** @file libe2dbg/kernel/syscalls.c
 **/

#include "ke2dbg.h"
#include "ke2dbg-syscalls.h"

static void **ke2dbg_syscalltable;

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
asmlinkage long (*ke2dbg_sys_rename)(const char *, const char *);
asmlinkage long (*ke2dbg_sys_munmap)(unsigned long, size_t);
asmlinkage long (*ke2dbg_sys_mmap2)(unsigned long, unsigned long, unsigned long, 
				    unsigned long, unsigned long , unsigned long);
asmlinkage long (*ke2dbg_sys_unlink)(const char *);
asmlinkage long (*ke2dbg_sys_mprotect)(unsigned long, size_t, unsigned long);
asmlinkage long (*ke2dbg_sys_wait4)(pid_t, int *, int, struct rusage *);
asmlinkage long (*ke2dbg_sys_waitpid)(pid_t, int *, int);
asmlinkage long (*ke2dbg_sys_access)(const char *, int);
asmlinkage long (*ke2dbg_sys_select)(int, fd_set *, fd_set *, fd_set *, struct timeval *);
asmlinkage long (*ke2dbg_sys_dup2)(unsigned int, unsigned int);
asmlinkage off_t (*ke2dbg_sys_lseek)(unsigned int, off_t, unsigned int);
asmlinkage long (*ke2dbg_sys_mkdir)(const char *, int);
asmlinkage long (*ke2dbg_sys_nanosleep)(struct timespec *, struct timespec *);

int ke2dbg_init_syscalltable(unsigned long value)
{
  ke2dbg_syscalltable = (void **)value;
  
  ke2dbg_sys_read	= ke2dbg_syscalltable[__NR_read];
  ke2dbg_sys_open	= ke2dbg_syscalltable[__NR_open];
  ke2dbg_sys_close	= ke2dbg_syscalltable[__NR_close];
  ke2dbg_sys_signal	= ke2dbg_syscalltable[__NR_signal];
  ke2dbg_sys_execve	= ke2dbg_syscalltable[__NR_execve];
  ke2dbg_sys_fsync	= ke2dbg_syscalltable[__NR_fsync];
  ke2dbg_sys_readdir	= ke2dbg_syscalltable[__NR_readdir];
  ke2dbg_sys_fork	= ke2dbg_syscalltable[__NR_fork];
  ke2dbg_sys_exit	= ke2dbg_syscalltable[__NR_exit];
  ke2dbg_sys_rename	= ke2dbg_syscalltable[__NR_rename];
  ke2dbg_sys_munmap	= ke2dbg_syscalltable[__NR_munmap];
  ke2dbg_sys_mmap2	= ke2dbg_syscalltable[__NR_mmap2];
  ke2dbg_sys_unlink	= ke2dbg_syscalltable[__NR_unlink];
  ke2dbg_sys_mprotect	= ke2dbg_syscalltable[__NR_mprotect];
  ke2dbg_sys_wait4	= ke2dbg_syscalltable[__NR_wait4];
  ke2dbg_sys_waitpid	= ke2dbg_syscalltable[__NR_waitpid];
  ke2dbg_sys_access	= ke2dbg_syscalltable[__NR_access];
  ke2dbg_sys_select	= ke2dbg_syscalltable[__NR_select];
  ke2dbg_sys_dup2	= ke2dbg_syscalltable[__NR_dup2];
  ke2dbg_sys_lseek	= ke2dbg_syscalltable[__NR_lseek];
  ke2dbg_sys_mkdir	= ke2dbg_syscalltable[__NR_mkdir];
  ke2dbg_sys_nanosleep	= ke2dbg_syscalltable[__NR_nanosleep];

  return 0;
}

asmlinkage ssize_t sys_read(int fd, void *buf, size_t count)
{
  ssize_t r;
  mm_segment_t oldfs;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_read(fd, buf, count);

  set_fs(oldfs);
  return r;
}

asmlinkage ssize_t sys_write(int fd, const void *buf, size_t count)
{
  ssize_t r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_write(fd, buf, count);

  set_fs(oldfs);
  
  return r;
}

asmlinkage int sys_open(const char *pathname, int flags, int mode)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_open(pathname, flags, mode);

  set_fs(oldfs);
  
  return r;
}

asmlinkage int sys_close(int fd)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_close(fd);

  set_fs(oldfs);
  
  return r;
}

asmlinkage sighandler_t sys_signal(int signum, sighandler_t handler)
{
  sighandler_t r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_signal(signum, handler);

  set_fs(oldfs);
  
  return r;
}

asmlinkage int sys_execve(struct pt_regs regs)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_execve(regs);

  set_fs(oldfs);
  
  return r;
}

asmlinkage int sys_fsync(int fd)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_fsync(fd);

  set_fs(oldfs);
  
  return r;
}

asmlinkage int sys_readdir(unsigned int fd, struct dirent *dirp, unsigned int count)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_readdir(fd, dirp, count);

  set_fs(oldfs);
  
  return r;  
}

asmlinkage int sys_fork(struct pt_regs regs)
{
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_fork(regs);

  set_fs(oldfs);
  
  return r;  
}

asmlinkage long sys_exit(int error_code)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_exit(error_code);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_rename(const char *oldname, const char *newname)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_rename(oldname, newname);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_munmap(unsigned long addr, size_t len)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_munmap(addr, len);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_mmap2(unsigned long addr, unsigned long len,
                          unsigned long prot, unsigned long flags,
                          unsigned long fd, unsigned long pgoff)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_mmap2(addr, len, prot, flags, fd, pgoff);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_unlink(const char *pathname)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_unlink(pathname);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_mprotect(unsigned long start, size_t len, unsigned long prot)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_mprotect(start, len, prot);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_wait4(pid_t pid, int *stat_addr, int options, struct rusage *ru)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_wait4(pid, stat_addr, options, ru);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_waitpid(pid_t pid, int *stat_addr, int options)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_waitpid(pid, stat_addr, options);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_access(const char *filename, int mode)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_access(filename, mode);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_select(int n, fd_set *inp, fd_set *outp,
			   fd_set *exp, struct timeval *tvp)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_select(n, inp, outp, exp, tvp);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_dup2(unsigned int oldfd, unsigned int newfd)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_dup2(oldfd, newfd);

  set_fs(oldfs);
  
  return r;
}

asmlinkage off_t sys_lseek(unsigned int fd, off_t offset, unsigned int origin)
{
  off_t r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_lseek(fd, offset, origin);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_mkdir(const char *pathname, int mode)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_mkdir(pathname, mode);

  set_fs(oldfs);
  
  return r;
}

asmlinkage long sys_nanosleep(struct timespec *rqtp, struct timespec *rmtp)
{
  long r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_nanosleep(rqtp, rmtp);

  set_fs(oldfs);
  
  return r;
}
