/*
** @file kmem.c
** @ingroup libkernsh_user
**
*/
#if defined(__linux__)
#define _LARGEFILE64_SOURCE
#endif

#include "libkernsh.h"

/**
 * @brief Open /dev/kmem on Linux 2.6.X
 * @return 0 on success, -1 on return
 */
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
	0, -1);

  libkernshworld.root->iotype = ELFSH_IOTYPE_DEVKMEM26;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Close /dev/kmem on Linux 2.6.X
 * @return 0 on success, -1 on return
 */
int kernsh_closemem_kmem_linux_2_6()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("CLOSE KMEM 2.6\n");
#endif

  XCLOSE(libkernshworld.fd, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Read /dev/kmem on Linux 2.6.X
 * @param offset Offset to read memory
 * @param buf Read memory into the buf
 * @param size Count bytes to read
 * @return size on success, -1 on error
 */
int kernsh_readmema_kmem_linux_2_6(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XREAD(libkernshworld.fd, buf, size, -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, size);
}

/**
 * @brief Write in /dev/kmem on Linux 2.6.X
 * @param offset Offset to write memory
 * @param buf Write buf into memoryr
 * @param size Count bytes to write
 * @return size on success, -1 on error
 */
int kernsh_writemem_kmem_linux_2_6(elfshobj_t *file, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

#if defined(__linux__)
  XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
  
  XWRITE(libkernshworld.fd, buf, size, -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, size);
}

/**
 * @brief Open /dev/kmem on Linux 2.4.X
 * @return 0 on success, -1 on return
 */
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

  libkernshworld.root->iotype = ELFSH_IOTYPE_DEVKMEM24;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Close /dev/kmem on Linux 2.4.X
 * @return 0 on success, -1 on return
 */
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

/**
 * @brief Read /dev/kmem on Linux 2.4.X
 * @param offset Offset to read memory
 * @param buf Read memory into the buf
 * @param size Count bytes to read
 * @return size on success, -1 on error
 */
int kernsh_readmema_kmem_linux_2_4(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

  if (libkernshworld.mmap)
    {
      if (memcpy(buf, libkernshworld.ptr+offset, size) == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Memcpy failed !", -1);
    }
  else
    {
#if defined(__linux__)
      XLSEEK64(libkernshworld.fd, offset, SEEK_SET, -1);
#endif
      XREAD(libkernshworld.fd, buf, size, -1);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, size);
}

/**
 * @brief Write in /dev/kmem on Linux 2.4.X
 * @param offset Offset to write memory
 * @param buf Write buf into memoryr
 * @param size Count bytes to write
 * @return size on success, -1 on error
 */
int kernsh_writemem_kmem_linux_2_4(elfshobj_t *unused, eresi_Addr offset, void *buf, int size)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, size);
}
