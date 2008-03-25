/** 
 ** @file libe2dbg/kernel/syscalls.c
 **/
#include "ke2dbg.h"
#include <asm/processor.h>
#include <asm/uaccess.h>

ssize_t read(int fd, void *buf, size_t count)
{
  ssize_t r;
  mm_segment_t oldfs;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  r = sys_read(fd, buf, count);

  set_fs(oldfs);
  return r;
}

ssize_t write(int fd, const void *buf, size_t count)
{
  return 0;
}
