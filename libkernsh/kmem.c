/*
** kmem.c for libkernsh
**
** $Id: kmem.c,v 1.5 2007-07-31 12:31:54 pouik Exp $
**
*/
#if defined(__linux__)
#define _LARGEFILE64_SOURCE
#endif

#include "libkernsh.h"

/* Open /dev/kmem on Linux 2.6.X */
int kernsh_openmem_kmem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("OPEN KMEM 2.6\n");
#endif

  if (libkernshworld.mmap)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Mmap not supported with /dev/kmem", -1);
    }
	
  XOPEN(libkernshworld.fd, 
	LIBKERNSH_STRING_DEVICE_KMEM, 
	libkernshworld.fdmode, 
	0, 
	-1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Close /dev/kmem on Linux 2.6 */
int kernsh_closemem_kmem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("CLOSE KMEM 2.6\n");
#endif

  XCLOSE(libkernshworld.fd, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Read /dev/kmem on Linux 2.6.X */
int kernsh_readmem_kmem_linux_2_6(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("READ KMEM 2.6\n");

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XREAD(libkernshworld.fd, buf, size, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Write in /dev/kmem on Linux 2.6.X */
int kernsh_writemem_kmem_linux_2_6(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("WRITE KMEM 2.6\n");

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XWRITE(libkernshworld.fd, buf, size, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Open /dev/kmem on Linux 2.4.X */
int kernsh_openmem_kmem_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("OPEN KMEM 2.4\n");
#endif

  XOPEN(libkernshworld.fd, 
	LIBKERNSH_STRING_DEVICE_KMEM, 
	libkernshworld.fdmode, 
	0, 
	-1);

  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMMAP(libkernshworld.ptr, 
	    0, 
	    libkernshworld.mmap_size, 
	    libkernshworld.protmode,
	    libkernshworld.flagsmode, 
	    libkernshworld.fd, 
	    0, 
	    -1);
#endif
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Close /dev/kmem on Linux 2.4 */
int kernsh_closemem_kmem_linux_2_4()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("CLOSE KMEM 2.4\n");
#endif
  
  if (libkernshworld.mmap)
    {
#if defined(__linux__)
      XMUNMAP(libkernshworld.ptr, libkernshworld.mmap_size, -1);
#endif
    }

  XCLOSE(libkernshworld.fd, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Read /dev/kmem on Linux 2.4.X */
int kernsh_readmem_kmem_linux_2_4(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("READ KMEM 2.4\n");


  if (libkernshworld.mmap)
    {
      if (memcpy(buf, libkernshworld.ptr+offset, size) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memcpy failed !", -1);
	}
    }
  else
    {
#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  XREAD(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Write in /dev/kmem on Linux 2.4.X */
int kernsh_writemem_kmem_linux_2_4(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("WRITE KMEM 2.4\n");

  if (libkernshworld.mmap)
    {
      if (memcpy(libkernshworld.ptr+offset, buf, size) == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memcpy failed !", -1);
	}
    }
  else
    {
#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  XWRITE(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
