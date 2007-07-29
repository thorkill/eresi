/*
** kmem.c for libkernsh
**
** $Id: kmem.c,v 1.4 2007-07-29 16:54:36 pouik Exp $
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
