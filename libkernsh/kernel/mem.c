#include "libkernsh-kernel.h"

int valid_phys_addr_range(unsigned long addr, size_t count)
{
  if (addr + count > __pa(high_memory))
    return 0;
  
  return 1;
}

int asmlinkage kernsh_read_mem(unsigned long addr, char *buffer, int len, int mode)
{
  ssize_t read, sz;
  char *ptr; 
  unsigned long p = addr;

  printk(KERN_ALERT "[+] kernsh_read_mem ENTER !!\n");

  if (!valid_phys_addr_range(p, len))
    return -EFAULT;

  read = 0;
  while (len > 0) 
    {
      /*
       * Handle first page in case it's not aligned
       */
      if (-p & (PAGE_SIZE - 1))
	sz = -p & (PAGE_SIZE - 1);
      else
	sz = PAGE_SIZE;
      
      sz = min_t(unsigned long, sz, len);
      
      ptr = xlate_dev_mem_ptr(p);
      
      printk(KERN_ALERT "SZ %d PTR 0x%lx\n", sz, (unsigned long)ptr);
      
      switch(mode)
	{
	case LIBKERNSH_PROC_MODE :
	  memcpy(buffer, ptr, sz);
	  break;
	case LIBKERNSH_SYSCALL_MODE :
	  if (copy_to_user(buffer, ptr, sz))
	    return -EFAULT;
	  break;
	}
      
      buffer += sz;
      p += sz;
      len -= sz;
      read += sz;
    }
  
  printk(KERN_ALERT "[+] kernsh_read_mem EXIT !!\n");

  return read;
}

int asmlinkage kernsh_write_mem(unsigned long addr, const char *buffer, int len, int mode)
{
  unsigned long p = addr;
  ssize_t written, sz;
  unsigned long copied;
  void *ptr;
  
  printk(KERN_ALERT "[+] kernsh_write_mem ENTER !!\n");

  if (!valid_phys_addr_range(p, len))
    return -EFAULT;
  
  written = 0;
  
  while (len > 0) 
    {
      /*
       * Handle first page in case it's not aligned
       */
      if (-p & (PAGE_SIZE - 1))
	sz = -p & (PAGE_SIZE - 1);
      else
	sz = PAGE_SIZE;
      
      sz = min_t(unsigned long, sz, len);
      
      /*
       * On ia64 if a page has been mapped somewhere as
       * uncached, then it must also be accessed uncached
       * by the kernel or data corruption may occur
       */
      ptr = xlate_dev_mem_ptr(p);
      
      switch(mode)
	{
	case LIBKERNSH_PROC_MODE :
	  memcpy(ptr, buffer, sz);
	  copied = sz;
	  break;
	case LIBKERNSH_SYSCALL_MODE :
	  copied = copy_from_user(ptr, buffer, sz);
	  break;
	}
      
      if (copied) 
	{
	  written += sz - copied;
	  if (written)
	    break;
	  
	  return -EFAULT;
	}
      
      buffer += sz;
      p += sz;
      len -= sz;
      written += sz;
    }
  
  printk(KERN_ALERT "[+] kernsh_write_mem EXIT !!\n");

  return written;
}
