/*
** @file idt.c
** @ingroup libkernsh_common
**
*/
#include "libkernsh.h"

/**
 * @brief Get the gdt
 * @param lgdt List to store the gdt
 * @return 0 on success, -1 on return
 */
/* Put interrupts inside the list lgdt */
int kernsh_gdt(list_t *lgdt)
{
  int ret;
  unsigned int         dim[3];
  vector_t      *gdt;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if(lgdt == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "List is NULL !", -1);
    }
  
  gdt = aspect_vector_get(LIBKERNSH_VECTOR_NAME_GDT);
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(gdt, dim);

  ret = fct(lgdt);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get the gdt on Linux
 * @param lgdt List to store the gdt
 * @return 0 on success, -1 on return
 */
int kernsh_gdt_linux(list_t *lgdt)
{
  int i;
  char *key;
  libkernshsgdt_t *sgdt;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("GDT LINUX\n");
#endif

  /* Interrupts is not set in static kernel ! */
  if (kernsh_is_static_mode())
    {
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to get gdt in static mode !", -1);
    }
  else 
    {
      if (!libkernshworld.open)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memory not open !", -1);
	}

      for (i = 0;
	   i < libkernshworld.gdt_limit;
	   i = i + 8)
	{
	  	  
	  XALLOC(__FILE__, 
		 __FUNCTION__, 
		 __LINE__, 
		 sgdt,
		 sizeof(libkernshsgdt_t), 
		 -1);

	  XALLOC(__FILE__, 
		 __FUNCTION__, 
		 __LINE__, 
		 key,
		 BUFSIZ, 
		 -1);
	  
	  memset(sgdt, '\0', sizeof(libkernshsgdt_t)); 
	  memset(key, '\0', BUFSIZ);

	  sgdt->addr = libkernshworld.gdt_base+i;
	  kernsh_readmem(libkernshworld.gdt_base+i,
			 &sgdt->deb,
			 sizeof(unsigned long));

	  kernsh_readmem(libkernshworld.gdt_base+i+4,
			 &sgdt->fin,
			 sizeof(unsigned long));

	  snprintf(key,
		   BUFSIZ,
		   "%d",
		   i);

	  
	  /* Add the segment in the list */
	  elist_add(lgdt, key, (void *) sgdt);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

/**
 * @brief Get the gdt on Netbsd
 * @param lgdt List to store the gdt
 * @return 0 on success, -1 on return
 */
int kernsh_gdt_netbsd(list_t *lgdt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Get the gdt on Freebsd
 * @param lgdt List to store the gdt
 * @return 0 on success, -1 on return
 */
int kernsh_gdt_freebsd(list_t *lgdt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
