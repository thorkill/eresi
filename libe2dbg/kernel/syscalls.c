/** 
 ** @file libe2dbg/kernel/syscalls.c
 **/
#include "ke2dbg.h"
#include "ke2dbg-syscalls.h"

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
  int r;
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
  int r;
  mm_segment_t oldfs = get_fs();

  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_exit(error_code);

  set_fs(oldfs);
  
  return r;
}

/*

OK signal
??? execvp
OK fsync
OK readdir
OK fork
OK exit
rename
munmap
unlink
mprotect
closedir
mmap
wait
access
select
dup2
lseek
opendir
mkdir
sleep

 */
