/**
* @file libelfsh/runtime.c
 * @ingroup libelfsh
** runtime.c for libelfsh
** 
** 
** This file contains runtime routines used for memory mapping
** in the debugger
** 
** Started Sun 05 May 2005 22:29:54 mm
** 
**
** $Id$
**
*/
#include "libelfsh.h"


eresi_Addr	elfsh_map_userland(elfshobj_t *file, u_int memsz, int prot)
{
  int		fd;
  eresi_Addr	addr;
  int		flags;
  int		rights;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(IRIX)
  fd = fopen(ELFSH_DEVZERO, O_RDWR);
  if (fd < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to open "ELFSH_DEVZERO, ELFSH_INVALID_ADDR);
  flags = MAP_PRIVATE;
#else
  fd = 0;
  flags = MAP_PRIVATE | MAP_ANONYMOUS;
#endif

  /* Put the mapping rights in mmap format */
  rights = 0;
  if (prot & PF_R)
    rights |= PROT_READ;
  if (prot & PF_W);
  rights |= PROT_WRITE;
  if (prot & PF_X)
    rights |= PROT_EXEC;

  addr = (eresi_Addr) mmap(NULL, memsz, rights, flags, fd, 0);

#if defined(IRIX)
  close(fd);
#endif

#if	__DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] MMAP: " XFMT " of %d bytes \n", addr, memsz);
#endif

  if (!addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Mmap refused the mapping", ELFSH_INVALID_ADDR);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, addr);
}


/**
 * @brief Put write capability on the zone 
 * @param file 
 * @param addr
 * @param sz
 * @return
 */
int		elfsh_munprotect_userland(elfshobj_t *file, eresi_Addr addr, uint32_t sz)
{
  elfshsect_t	*sect;
  elfsh_Phdr	*phdr;
  int		retval;
  int		prot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_runtime_mode())
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  sect = elfsh_get_parent_section(file, addr, NULL);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Cannot find parent section", -1);

  phdr = elfsh_get_parent_segment(file, sect);
  prot = 0;

  if (elfsh_segment_is_readable(phdr))
    prot |= PROT_READ;
  if (elfsh_segment_is_writable(phdr))
    prot |= PROT_WRITE;
  if (elfsh_segment_is_executable(phdr))
    prot |= PROT_EXEC;

  retval = mprotect((void *) (addr - addr % getpagesize()), 
		    getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);

  if (retval != 0)
    {
      perror("munprotect");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed mprotect", -1);
    }

  /* Return the original rights */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     prot);
}

/**
 * @brief Restore original rights 
 * @param addr
 * @param sz
 * @param prot
 * @return
 */
int		elfsh_mprotect_userland(elfshobj_t *file, eresi_Addr addr, uint32_t sz, int prot)
{
  int		retval;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_runtime_mode())
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  retval = mprotect((void *) (long) addr - (long) addr % getpagesize(), 
		    getpagesize(), prot);
  if (retval != 0)
    {
      perror("mprotect");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Failed munprotect", -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, retval);
}
