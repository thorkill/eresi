/*
** kmem.c for libkernsh
**
** $Id: kmem.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#if defined(__linux__)
#define _LARGEFILE64_SOURCE
#endif

#include "libkernsh.h"

int kernsh_openmem_kmem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("OPEN KMEM 2.6\n");

  if (libkernshworld.mmap)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Mmap not supported with /dev/kmem", -1);
    }
	
  XOPEN(libkernshworld.fd, "/dev/kmem", libkernshworld.fdmode, 0, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_closemem_kmem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("CLOSE KMEM 2.6\n");

  XCLOSE(libkernshworld.fd, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_readmem_kmem_linux_2_6(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("READ KMEM 2.6\n");

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XREAD(libkernshworld.fd, buf, size, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_writemem_kmem_linux_2_6(unsigned long offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("WRITE KMEM 2.6\n");

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XWRITE(libkernshworld.fd, buf, size, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
