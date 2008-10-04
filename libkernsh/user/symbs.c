/*
** @file symbs.c
** @ingroup libkernsh_user
**
*/
#include "libkernsh.h"
#include "libkernsh-info.h"

#if __LINUX_2_4__
#include <linux/module.h>
#endif

/**
 * @brief Get a symbol's address
 * @param name Symbol's name
 * @param addr Symbol's address
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_addr_by_name(char *name, unsigned long *addr, size_t size)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *symbs;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  symbs = aspect_vector_get(LIBKERNSH_VECTOR_NAME_ADDRBYNAME);
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(symbs, dim);

  ret = fct(name, addr, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get a symbol's name
 * @param addr Symbol's address
 * @param name Symbol's name
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_name_by_addr(unsigned long addr, char *name, size_t size)
{
  int ret;
  unsigned int         dim[2];
  vector_t      *symbs;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  symbs = aspect_vector_get(LIBKERNSH_VECTOR_NAME_NAMEBYADDR);
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(symbs, dim);

  ret = fct(addr, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get a symbol's address with kstrtab method, based on phalanx rootkit :)
 * @param symbol Symbol's name
 * @param addr Symbol's address
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_walk_kstrtab(const char *symbol, unsigned long *addr, size_t size)
{
  char srch[512];
  char tab[] = { '\0', '\xff' };
  unsigned long kstrtab, x, i;
  int j;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  kstrtab = x = i = j = 0;
  size += 2;
  
  memset(srch, '\0', sizeof(srch));
  memcpy (srch + 1, symbol, size);
  srch[size] = '\0';

  *addr = 0;
  while(j < 2)
    {
      srch[0] = tab[j];
      for (x = 0, i = 0; 
	   x < libkernshworld.kernel_end - libkernshworld.kernel_start; 
	   x++, i++)
	{

#if __DEBUG_KERNSH__  
	  if (i % 500000 == 0)
	    write (1, "?", 1);
	  else if (i == 4000001)
	    {
	      write (1, "\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b\b", 24);
	      i = 0;
	    }
#endif
	  /* Ok we have find kstrtab_name */
	  if(memcmp ((unsigned char *) (libkernshworld.ptr + x), 
		     srch, 
		     size) == 0)
	    {
	      kstrtab = libkernshworld.kernel_start + x + 1;
	      break;
	    }
	}
      j++;
      srch[0] = tab[j];

#if __DEBUG_KERNSH__
      printf("kstrtab = 0x%.8lx\n", (unsigned long)kstrtab);
#endif

      if(kstrtab != 0)
	{
	  /* Now we search ksymtab, paper street is not so far */
	  for (x = 0, i = 0; 
	       x < libkernshworld.kernel_end - libkernshworld.kernel_start; 
	       x++, i++)
	    {
#if __DEBUG_KERNSH__	    
	      if (i % 500000 == 0)
		write (1, "?", 1);
	      else if(i == 4000001)
		{
		  write (1, "\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b\b", 24);
		  i = 0;
		}
#endif
	      /* We find ksymtab_name, uhm on some kernel the kstrtab 
		 is present twice, so we keep the last */
	      if (*(unsigned long *) (libkernshworld.ptr + x) == kstrtab)
		{
#if __DEBUG_KERNSH__
		  printf("ksymtab 0x%lx\n", (libkernshworld.kernel_start + x - sizeof(unsigned long)));
#endif
		  *addr = *(unsigned long *) (libkernshworld.ptr + x - sizeof(unsigned long));
		}
	    }
	}
      kstrtab = 0;
      j++;
      srch[0] = tab[j];
    }

  if (*addr != 0)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to find symbol", -1);
}

/**
 * @brief Get a symbol's address on Linux 2.6.X
 * @param name Symbol's name
 * @param addr Symbol's address
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_addr_by_name_linux_2_6(char *name, unsigned long *addr, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First, check if mmap is active, find a symbol by kstrtab */
  if(libkernshworld.mmap)
    {
      ret = kernsh_walk_kstrtab(name, addr, size);
      /* We didn't find it, use lame system map:( */
      if(ret)
	{
	  ret = kernsh_rresolve_systemmap(name, addr, size);
	}
    }
  /* Else we use lame system map :( */
  else
    {
      ret = kernsh_rresolve_systemmap(name, addr, size);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get a symbol's name on Linux 2.6.X
 * @param addr Symbol's address
 * @param name Symbol's name
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_name_by_addr_linux_2_6(unsigned long addr, char *name, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_resolve_systemmap(addr, name, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get a symbol's address with kernel syms
 * @param name Symbol's name
 * @param addr Symbol's address
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_kernel_syms(char *name, unsigned long *addr, size_t size)
{
#if defined(__linux__)
#if __LINUX_2_4__
  int i, numsyms;
  struct kernel_sym tab_kernel_sym[8192];
#endif
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(__linux__)
#if __LINUX_2_4__ 
  numsyms = get_kernel_syms(NULL);
  if (numsyms < 0 || numsyms > 8192)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Can't get symbol by get_kernel_syms", -1);
    }

  get_kernel_syms(tab_kernel_sym);

  for (i = 0; i < numsyms; i++)
    {
      if (!strncmp(name, tab_kernel_sym[i].name, size))
	{
	  *addr = tab_kernel_sym[i].value;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
    }
#endif
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);
}

/**
 * @brief Get a symbol's address on Linux 2.4.X
 * @param name Symbol's name
 * @param addr Symbol's address
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_addr_by_name_linux_2_4(char *name, unsigned long *addr, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_get_kernel_syms(name, addr, size);

  if (ret)
    ret = kernsh_rresolve_systemmap(name, addr, size);
   
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get a symbol's name on Linux 2.4.X
 * @param addr Symbol's address
 * @param name Symbol's name
 * @param size Symbol's name size
 * @return 0 on success, -1 on return
 */
int kernsh_get_name_by_addr_linux_2_4(unsigned long addr, char *name, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = kernsh_resolve_systemmap(addr, name, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
