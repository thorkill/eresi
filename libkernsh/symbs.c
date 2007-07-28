/*
** symbs.c for libkernsh
**
** $Id: symbs.c,v 1.4 2007-07-28 15:02:23 pouik Exp $
**
*/
#include "libkernsh.h"

/* Get an addr by name */
int kernsh_get_addr_by_name(char *name, unsigned long *addr, size_t size)
{
  int ret;
  u_int         dim[2];
  vector_t      *symbs;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("SYMBS ABN\n");

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  symbs = aspect_vector_get("addr_by_name");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(symbs, dim);

  ret = fct(name, addr, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Get name by addr */
int kernsh_get_name_by_addr(unsigned long addr, char *name, size_t size)
{
  int ret;
  u_int         dim[2];
  vector_t      *symbs;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

//  printf("SYMBS NBA\n");

  if (!libkernshworld.open)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memory not open !", -1);
    }

  symbs = aspect_vector_get("name_by_addr");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(symbs, dim);

  ret = fct(addr, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* This function resolve a symbol with kstrtab methode : based on phalanx rootkit :) */
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
	  if(memcmp ((unsigned char *) (libkernshworld.ptr + x), srch, size) == 0)
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
	      if (*(unsigned long *) (libkernshworld.ptr + x) == kstrtab)
		{
		  *addr = *(unsigned long *) (libkernshworld.ptr + x - sizeof(unsigned long));
		  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
		}
	    }
	}
      kstrtab = 0;
      j++;
      srch[0] = tab[j];
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to find symbol", -1);
}

int kernsh_get_addr_by_name_linux_2_6(char *name, unsigned long *addr, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("SYMBS ABN LINUX 2.6\n");

  if(libkernshworld.mmap)
    {
      ret = kernsh_walk_kstrtab(name, addr, size);
      if(ret)
	{
	  ret = kernsh_rresolve_systemmap(name, addr, size);
	}
    }
  else
    {
      ret = kernsh_rresolve_systemmap(name, addr, size);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_get_name_by_addr_linux_2_6(unsigned long addr, char *name, size_t size)
{
  int ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("SYMBS NBA LINUX 2.6\n");

  ret = kernsh_resolve_systemmap(addr, name, size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
