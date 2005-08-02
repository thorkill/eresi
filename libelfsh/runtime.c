/*
** runtime.c for libelfsh
**
**
** This file contains runtime routines used for memory mapping
** in the debugger
**
** Started Sun 05 May 2005 22:29:54 mm
**
*/
#include "libelfsh.h"



/* Set PHT entry rights from elfsh section mode */
int		elfsh_set_phdr_prot(u_int mode)
{
  elfsh_Word	flags;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  flags = PF_R | PF_W;
  // if (mode == ELFSH_CODE_INJECTION)
    flags |= PF_X;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (flags));
}

/* Map a new area in memory */
elfsh_Addr	 elfsh_runtime_map(elfsh_Phdr *segment)
{
  elfsh_Addr	addr;
  int		prot = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_segment_is_readable(segment))
    prot |= PROT_READ;
  if (elfsh_segment_is_writable(segment))
    prot |= PROT_WRITE;
  if (elfsh_segment_is_executable(segment))
    prot |= PROT_EXEC;

#if	__DEBUG_RUNTIME__
  printf("[DEBUG_RUNTIME] MMAP: " XFMT " of %d bytes \n", segment->p_vaddr, segment->p_memsz);
#endif
  
  addr = (elfsh_Addr) mmap((void *) 0, segment->p_memsz, 
			   prot,
			   //elfsh_get_segment_flags(segment), 
			   MAP_PRIVATE | MAP_ANONYMOUS, 
			   0, 0);
  
  if (addr == 0 && segment->p_vaddr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Mmap refused the mapping", 
		      ELFSH_INVALID_ADDR);
#if	__DEBUG_RUNTIME__
  else
    printf("[DEBUG_RUNTIME] Section Mapped at addr %08X with prot %c%c%c\n", 
	   addr,
	   (elfsh_segment_is_readable(segment)   ? 'R' : '-'),
	   (elfsh_segment_is_writable(segment)   ? 'W' : '-'),
	   (elfsh_segment_is_executable(segment) ? 'X' : '-'));
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (addr));
}


/* Unmap a previously requested area */
int		elfsh_runtime_unmap(elfsh_Phdr *segment)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (munmap((void *) segment->p_vaddr, segment->p_memsz)));
}



/* Synchronize ondisk modifications in memory */
int		elfsh_runtime_sync()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Put write capability on the zone */
int		elfsh_munprotect(elfshobj_t *file, 
				 elfsh_Addr addr, 
				 uint32_t sz)
{
  elfshsect_t	*sect;
  elfsh_Phdr	*phdr;
  int		retval;
  int		prot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  sect = elfsh_get_parent_section(file, addr, NULL);
  if (!sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
		    getpagesize(), PROT_READ | PROT_WRITE);

  if (retval != 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed mprotect", -1);

  /* Return the original rights */
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     prot);
}



/* Restore original rights */
int		elfsh_mprotect(elfsh_Addr addr, uint32_t sz, int prot)
{
  int		retval;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  retval = mprotect((void *) addr - addr % getpagesize(), 
		    getpagesize(), prot);

  if (retval != 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Failed munprotect", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, retval);
}
