/** 
 ** @file libe2dbg/kernel/syscalls.c
 **/
#include "ke2dbg.h"
#include "ke2dbg-syscalls.h"

#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

static void **ke2dbg_syscalltable;

int ke2dbg_init_syscalltable(unsigned long value)
{
  ke2dbg_syscalltable = (void **)value;
  
  ke2dbg_sys_read = ke2dbg_syscalltable[__NR_read];
  ke2dbg_sys_open = ke2dbg_syscalltable[__NR_open];
  ke2dbg_sys_close = ke2dbg_syscalltable[__NR_close];

  return 0;
}

ssize_t read(int fd, void *buf, size_t count)
{
  ssize_t r;
  mm_segment_t oldfs;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_read(fd, buf, count);

  set_fs(oldfs);
  return r;
}

ssize_t write(int fd, const void *buf, size_t count)
{
  int r;
  mm_segment_t oldfs;
  
  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_write(fd, buf, count);
  set_fs(oldfs);
  
  return r;
}

int open(const char *pathname, int flags, int mode)
{
  int r;
  mm_segment_t oldfs;
  
  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_open(pathname, flags, mode);
  set_fs(oldfs);
  
  return r;
}

int close(int fd)
{
  int r;
  mm_segment_t oldfs;
  
  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = ke2dbg_sys_close(fd);
  set_fs(oldfs);
  
  return r;
}

