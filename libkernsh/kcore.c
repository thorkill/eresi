/*
** kcore.c for libkernsh
**
** $Id: kcore.c,v 1.4 2007-07-31 12:31:54 pouik Exp $
**
*/
#define _LARGEFILE64_SOURCE
#include "libkernsh.h"

int kernsh_openmem_kcore_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN KCORE 2.6\n");
#endif

  XOPEN(libkernshworld.fd, 
	LIBKERNSH_STRING_DEVICE_KCORE, 
	libkernshworld.fdmode, 
	0, 
	-1);

  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMMAP(libkernshworld.ptr, 
	    0, libkernshworld.mmap_size, 
	    libkernshworld.protmode,
	    libkernshworld.flagsmode, 
	    libkernshworld.fd, 
	    0, 
	    -1);
#endif
    }

  libkernshworld.open = 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_kcore_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_KERNSH__
  printf("CLOSE KCORE 2.6\n");
#endif

  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMUNMAP(libkernshworld.ptr, libkernshworld.mmap_size, -1);
#endif
    }

  XCLOSE(libkernshworld.fd, -1);

  libkernshworld.open = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_kcore_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("READ KCORE 2.6\n");

  roffset = offset - libkernshworld.kernel_start + 0x1000;

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

int kernsh_writemem_kcore_linux_2_6(unsigned long offset, void *buf, int size)
{
  unsigned long roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("WRITE KCORE 2.6\n");

  roffset = offset - libkernshworld.kernel_start + 0x1000;

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
#if defined (__linux__)
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, -1);
#endif
      XWRITE(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
