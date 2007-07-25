/*
** kcore.c for libkernsh
**
** $Id: kcore.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#define _LARGEFILE64_SOURCE
#include "libkernsh.h"

int kernsh_openmem_kcore_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("OPEN KCORE 2.6\n");
  
  XOPEN(libkernshworld.fd, "/proc/kcore", libkernshworld.fdmode, 0, -1);

  if (libkernshworld.mmap)
    {
      XMMAP(libkernshworld.ptr, 0, libkernshworld.mmap_size, libkernshworld.protmode,
	    libkernshworld.flagsmode, libkernshworld.fd, 0, -1);
    }

  libkernshworld.open = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_kcore_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  printf("CLOSE KCORE 2.6\n");

  if (libkernshworld.mmap)
    {
      XMUNMAP(libkernshworld.ptr, libkernshworld.mmap_size, -1);
    }

  XCLOSE(libkernshworld.fd, -1);

  libkernshworld.open = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_kcore_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("READ KCORE 2.6\n");

  roffset = offset - libkernshworld.page_offset + 0x1000;

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
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, -1);
      XREAD(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_writemem_kcore_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("WRITE KCORE 2.6\n");

  roffset = offset - libkernshworld.page_offset + 0x1000;

  if (libkernshworld.mmap)
    {
      if (memcpy(libkernshworld.ptr+roffset, buf, size) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Memcpy failed !", -1);
	}
       
      XMSYNC(libkernshworld.ptr, 200 * 1024 * 1024, MS_SYNC, -1);
    }
  else
    {
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, -1);
      XWRITE(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
