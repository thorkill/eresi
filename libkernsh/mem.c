/*
** mem.c for libkernsh
**
** $Id: mem.c,v 1.2 2007-07-25 21:55:06 pouik Exp $
**
*/
#if defined(__linux__)
#define _LARGEFILE64_SOURCE
#endif

#include "libkernsh.h"

int kernsh_openmem_mem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("OPEN MEM 2.6\n");
  
  XOPEN(libkernshworld.fd, "/dev/mem", libkernshworld.fdmode, 0, -1);

  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMMAP(libkernshworld.ptr, 0, libkernshworld.mmap_size, libkernshworld.protmode,
	    libkernshworld.flagsmode, libkernshworld.fd, 0, -1);
#endif
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_mem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("CLOSE MEM 2.6\n");

  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMUNMAP(libkernshworld.ptr, libkernshworld.mmap_size, -1);
#endif
    }

  XCLOSE(libkernshworld.fd, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_mem_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("READ MEM 2.6\n");

  roffset = offset - libkernshworld.page_offset;

  if (libkernshworld.mmap)
    {
      if (memcpy(buf, libkernshworld.ptr+roffset, size) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memcpy failed !", -1);
	}
    }
  else
    {
#if defined(__linux__)
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, -1);
#endif
      XREAD(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_writemem_mem_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("WRITE MEM 2.6\n");

  roffset = offset - libkernshworld.page_offset;

  if (libkernshworld.mmap)
    {
      if (memcpy(libkernshworld.ptr+roffset, buf, size) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Memcpy failed !", -1);
	}
#if defined(__linux__)       
      XMSYNC(libkernshworld.ptr, libkernshworld.mmap_size, MS_SYNC, -1);
#endif
    }
  else
    {
#if defined(__linux__)
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, -1);
#endif
      XWRITE(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_openmem_netbsd()
{
#if defined(__NetBSD__)
  char errbuf[_POSIX2_LINE_MAX];
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("OPEN MEM NETBSD\n");

#if defined(__NetBSD__)
  libkernshworld.kd = krevm_openfiles(NULL, NULL, NULL, libkernshworld.fdmode, errbuf);

  if(libkernshworld.kd == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   errbuf, -1);

    }

#endif

  libkernshworld.open = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("CLOSE MEM NETBSD\n");

#if defined(__NetBSD__)
  if(krevm_close(libkernshworld.kd) < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed to close memory", -1);
    }
#endif

  libkernshworld.open = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_netbsd(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("READ MEM NETBSD\n");

#if defined(__NetBSD__)
  if (krevm_read(libkernshworld.kd, offset, buf, size) < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   krevm_geterr(libkernshworld.kd), -1);
    }
#endif  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_writemem_netbsd(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("WRITE MEM NETBSD\n");

#if defined(__NetBSD__)
  if (krevm_write(libkernshworld.kd, offset, buf, size) < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   krevm_geterr(libkernshworld.kd), -1);
    }
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
