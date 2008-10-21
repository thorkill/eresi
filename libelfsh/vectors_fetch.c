/**
** @file vectors_fetch.c
** @ingroup libelfsh
**
** vectors_fetch.c for libelfsh (The ELF shell library)
** 
** API for fetching handlers from vectors of libelfsh
**
** $Id$
*/
#include "libelfsh.h"

static hash_t	interp_hash;
static u_char	elfsh_ostype[5] = {
  ELFOSABI_LINUX,		
  ELFOSABI_FREEBSD,	
  ELFOSABI_NETBSD,		
  ELFOSABI_OPENBSD,	
  ELFOSABI_SOLARIS,	
};


/**
 * Translate ELF architecture type into ELFsh architecture type 
 *
 * @param file
 * @return
 */
u_char		elfsh_get_archtype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
    "Invalid file argument!", (ELFSH_ARCH_ERROR));

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA32));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC32));
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC64));
    case EM_PARISC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PARISC));
    case EM_IA_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA64));
    case EM_PPC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC32));
    case EM_PPC64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC64));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ALPHA64));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_MIPS32));
    case EM_ARM:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ARM));
    case EM_X86_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_AMD64));
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ERROR));
    }
}


/**
 * Translate ELF object type into ELFsh object type 
 * @param file
 * @return
 */
u_char		elfsh_get_elftype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_objtype(file->hdr))
    {
    case ET_EXEC:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_FILE_EXEC));
    case ET_DYN:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_FILE_LIB));
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_FILE_ERROR));
    }
}

/**
 * Return IO type associated with this file
 * @param file
 * @return
 */
u_char		elfsh_get_iotype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file->iotype);
}

      
/**
 * Retreive ELFsh OS type from ELF header 
 * @param file
 * @return
 */
u_char  elfsh_get_real_ostype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (file->hdr->e_ident[EI_OSABI])
    {
    case ELFOSABI_LINUX:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_LINUX));
    case ELFOSABI_FREEBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_FREEBSD));
    case ELFOSABI_NETBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_NETBSD));
    case ELFOSABI_OPENBSD:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_OPENBSD));
    case ELFOSABI_SOLARIS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_SOLARIS));
    case 0:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_ERROR));
    }
}


/**
 * @brief Retreive the host type for a file
 * @param file The working file.
 * @return Host type for the file.
 */
u_char		elfsh_get_hosttype(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, file->hostype);
}


/**
 * Fill up ELFsh Operating System type 
 *
 * @param file
 * @return
 */
u_char		elfsh_get_ostype(elfshobj_t *file)
{
  u_char	*res;
  char		*interp;
  char		*end;
  char		r;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First try */
  r = elfsh_get_real_ostype(file);
  switch (r)
    {
      /* Information not present */
    case 0:
      break;
    default:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
    }
  
  /* Smart intermediate way, fingerprint by .interp section */
  /* Not aware of any other software using this technique */
  if (elfsh_get_interp(file))
    {
      if (!interp_hash.size)
	{
	  hash_init(&interp_hash, "interpreters", 10, ASPECT_TYPE_STR);
	  hash_add(&interp_hash, 
		   "/lib/ld-linux.so", 
		   &elfsh_ostype[ELFSH_OS_LINUX]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-elf.so", 
		   &elfsh_ostype[ELFSH_OS_FREEBSD]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-eld_so", 
		   &elfsh_ostype[ELFSH_OS_NETBSD]);
	  hash_add(&interp_hash, 
		   "??????????????", 
		   &elfsh_ostype[ELFSH_OS_OPENBSD]);
	  hash_add(&interp_hash, 
		   "/usr/lib/ld.so", 
		   &elfsh_ostype[ELFSH_OS_SOLARIS]);
	  
	}


      /* Only useful for objects with .interp section */
      /* e.g. : ET_EXEC, and some special ET_DYN like libc */
      /* Cannot use elfsh_readmem / elfsh_get_raw because we might have a reentrancy problem
	 or even the runtime base address might not yet be filled if we are called by the debugger */
      interp = (char *) file->secthash[ELFSH_SECTION_INTERP]->data;
      end = strstr(".so", interp);
      if (end)
	*(end + 3) = 0x00;
      res  = hash_get(&interp_hash, interp);
      if (res)
	{
	  file->hdr->e_ident[EI_OSABI] = *res;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     (elfsh_get_real_ostype(file)));
	}
    }

  /* Fatalist */
#if defined(__linux__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_LINUX);
#elif defined(__FreeBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_FREEBSD);
#elif defined(__NetBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_NETBSD);
#elif defined(__OpenBSD__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_OPENBSD);
#elif defined(sun)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_SOLARIS);
#elif defined(__BEOS__)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_BEOS);
#else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_ERROR);
#endif
}

/**
 * Return the page size 
 *
 * @param file
 * @return
 */
int       elfsh_get_pagesize(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
    case ELFSH_ARCH_MIPS32:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 4096);
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_ALPHA64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 8192);
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
    case ELFSH_ARCH_ERROR:
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}

/**
 * Return the page size 
 *
 * @param file
 * @return
 */
u_int       elfsh_get_breaksize(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    case ELFSH_ARCH_MIPS32:
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_ALPHA64:
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 4);
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_ERROR:
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}





