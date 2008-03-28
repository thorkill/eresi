/** 
 ** @file libe2dbg/kernel/libc-wrapper.c
 **/

#include "ke2dbg.h"
#include "ke2dbg-syscalls.h"

ssize_t read(int fd, void *buf, size_t count)
{
  return sys_read(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
  return sys_write(fd, buf, count);
}

int open(const char *pathname, int flags, int mode)
{
  return sys_open(pathname, flags, mode);
}

int close(int fd)
{
  return sys_close(fd);
}

sighandler_t signal(int signum, sighandler_t handler)
{
  return sys_signal(signum, handler);
}

int fsync(int fd)
{
  return sys_fsync(fd);
}

int readdir(unsigned int fd, struct dirent *dirp, unsigned int count)
{
  return sys_readdir(fd, dirp, count);
}

//FIXME int sys_fork(struct pt_regs regs)

void exit(int error_code)
{
  sys_exit(error_code);
  return;
}

int rename(const char *oldname, const char *newname)
{
  return sys_rename(oldname, newname);
}

int munmap(unsigned long addr, size_t len)
{
  return sys_munmap(addr, len);
}

/*FIXME
long mmap(unsigned long addr, unsigned long len,
	  unsigned long prot, unsigned long flags,
	  unsigned long fd, unsigned long pgoff)
{
  return sys_mmap2(addr, len, prot, flags, fd, pgoff);
  }*/

int unlink(const char *pathname)
{
  return sys_unlink(pathname);
}

int mprotect(unsigned long start, size_t len, unsigned long prot)
{
  return sys_mprotect(start, len, prot);
}

// FIXME long sys_wait4(pid_t pid, int *stat_addr, int options, struct rusage *ru)

// FIXME long sys_waitpid(pid_t pid, int *stat_addr, int options)


int access(const char *filename, int mode)
{
  return sys_access(filename, mode);
}

//FIXME asmlinkage long sys_access(const char *, int);
//FIXME asmlinkage long sys_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
//FIXME asmlinkage long sys_dup2(unsigned int, unsigned int);
//FIXME asmlinkage off_t sys_lseek(unsigned int, off_t, unsigned int);
//FIXME asmlinkage long sys_mkdir(const char *, int);
//FIXME asmlinkage long sys_nanosleep(struct timespec *, struct timespec *);
