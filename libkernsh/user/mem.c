/**
* @file libkernsh/user/mem.c
** @ingroup usermode
**
*/
#if defined(__linux__)
#define _LARGEFILE64_SOURCE
#endif

#include "libkernsh.h"

/**
 * @brief Open /dev/mem on Linux 2.X
 * @return 0 on success, -1 on return
 */
int kernsh_openmem_mem_linux()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("OPEN MEM 2.X\n");
#endif

  XOPEN(libkernshworld.fd, 
	LIBKERNSH_STRING_DEVICE_MEM, 
	libkernshworld.fdmode, 
	0, 
	-1);

  /* If mmap is specified */
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

  libkernshworld.physical = 1;
  libkernshworld.root->iotype = ELFSH_IOTYPE_DEVMEM26;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Close /dev/mem on Linux 2.X
 * @return 0 on success, -1 on return
 */
int kernsh_closemem_mem_linux()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__  
  printf("CLOSE MEM 2.X\n");
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

/**
 * @brief Read /dev/mem on Linux 2.X
 * @param offset Offset to read memory
 * @param buf Read memory into the buf
 * @param size Count bytes to read
 * @return size on success, -1 on error
 */
void		*kernsh_readmema_mem_linux(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  eresi_Addr	roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", NULL);
  if (elfsh_is_static_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot read /dev/mem in static mode", NULL);

  /* We must substract kernel_start(page_offset) to get the physical address */
  roffset = offset - libkernshworld.kernel_start;
  if (libkernshworld.mmap)
    {
      if (memcpy(buf, libkernshworld.ptr + roffset, size) == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Memcpy failed !", NULL);
    }
  else
    {
#if defined(__linux__)
      XLSEEK64(libkernshworld.fd, roffset, SEEK_SET, NULL);
#endif
      XREAD(libkernshworld.fd, buf, size, NULL);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}

/**
 * @brief Write into /dev/mem on Linux 2.X
 * @param offset Offset to write memory
 * @param buf Write buf into memoryr
 * @param size Count bytes to write
 * @return size on success, -1 on error
 */
int		kernsh_writemem_mem_linux(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  eresi_Addr	roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

  /* We must subtrack kernel_start (page_offset) to get the physical address */
  roffset = offset - libkernshworld.kernel_start;

  if (libkernshworld.mmap)
    {
      if (memcpy(libkernshworld.ptr+roffset, buf, size) == NULL)
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
      XWRITE(libkernshworld.fd, buf, size, -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, size);
}

int kernsh_openmem_netbsd()
{
#if defined(__NetBSD__)
  char errbuf[_POSIX2_LINE_MAX];
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("OPEN MEM NETBSD\n");
#endif

#if defined(__NetBSD__)
  libkernshworld.kd = krevm_openfiles(NULL, 
				      NULL, 
				      NULL, 
				      libkernshworld.fdmode, 
				      errbuf);

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

#if __DEBUG_KERNSH__  
  printf("CLOSE MEM NETBSD\n");
#endif

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

void	*kernsh_readmema_netbsd(elfshobj_t *file, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", NULL);
  if (elfsh_is_static_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot read /dev/mem in static mode", NULL);

  //  printf("READ MEM NETBSD\n");

#if defined(__NetBSD__)
  if (krevm_read(libkernshworld.kd, offset, buf, size) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 krevm_geterr(libkernshworld.kd), NULL);
#endif  

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}

int kernsh_writemem_netbsd(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

  //  printf("WRITE MEM NETBSD\n");

#if defined(__NetBSD__)
  if (krevm_write(libkernshworld.kd, offset, buf, size) < 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   krevm_geterr(libkernshworld.kd), -1);
    }
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
