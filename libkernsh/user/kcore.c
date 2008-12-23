/*
** @file kcore.c
** @ingroup libkernsh_user
**
*/
#define _LARGEFILE64_SOURCE
#include "libkernsh.h"

/**
 * @brief Open /proc/kcore on Linux
 * @return 0 on success, -1 on return
 */
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
  libkernshworld.root->iotype = ELFSH_IOTYPE_KCORE;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Close /proc/kcore on Linux
 * @return 0 on success, -1 on return
 */
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

/**
 * @brief Read /proc/kcore on Linux
 * @param offset Offset to read memory
 * @param buf Read memory into the buf
 * @param size Count bytes to read
 * @return size on success, -1 on error
 */
void		*kernsh_readmema_kcore_linux_2_6(elfshobj_t *unused, eresi_Addr offset, 
						 void *buf, int size)
{
  eresi_Addr	roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", NULL);

  roffset = offset - libkernshworld.kernel_start + 0x1000;

  if (libkernshworld.mmap)
    {
      if (memcpy(buf, libkernshworld.ptr+roffset, size) == NULL)
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
 * @brief Write into /proc/kcore
 * @param offset Offset to write memory
 * @param buf Write buf into memoryr
 * @param size Count bytes to write
 * @return size on success, -1 on error
 */
int		kernsh_writemem_kcore_linux_2_6(elfshobj_t *unused, eresi_Addr offset, void *buf, 
						int size)
{
  eresi_Addr	roffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!libkernshworld.open)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Memory not opened !", -1);

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
