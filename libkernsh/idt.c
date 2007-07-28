/*
** idt.c for libkernsh
**
** $Id: idt.c,v 1.3 2007-07-28 15:02:23 pouik Exp $
**
*/
#include "libkernsh.h"

/* Put idt inside the list lidt */
int kernsh_idt(list_t *lidt)
{
  int ret;
  u_int         dim[3];
  vector_t      *idt;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if(lidt == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "List is NULL !", -1);
    }
  
  idt = aspect_vector_get("idt");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(idt, dim);

  ret = fct(lidt);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


int kernsh_idt_linux(list_t *lidt)
{
  int i;
  char *key;
  libkernshint_t *dint;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("IDT LINUX\n");
#endif

  if (kernsh_is_static_mode())
    {
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to get idt in static mode !", -1);
    }
  else 
    {
      if (!libkernshworld.open)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memory not open !", -1);
	}

      for (i = (libkernshworld.idt_limit + 1) / (sizeof(unsigned long) * 2) - 1;
	   i >= 0;
	   i--)
	{
	  kernsh_readmem(libkernshworld.idt_base+sizeof(unsigned long)*2*i,
			 &idt,
			 sizeof(idt));

	  XALLOC(__FILE__, 
		 __FUNCTION__, 
		 __LINE__, 
		 dint,
		 sizeof(libkernshint_t), 
		 -1);

	  XALLOC(__FILE__, 
		 __FUNCTION__, 
		 __LINE__, 
		 key,
		 BUFSIZ, 
		 -1);

	  memset(dint, '\0', sizeof(libkernshint_t)); 
	  memset(key, '\0', BUFSIZ);
	  snprintf(key,
		   BUFSIZ,
		   "%d",
		   i);

	  dint->addr = (unsigned long)(idt.off2 << 16) + idt.off1;
	  kernsh_resolve_systemmap(dint->addr, 
				   dint->name, 
				   sizeof(dint->name));
	  
	  list_add(lidt, key, (void *) dint);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

int kernsh_idt_netbsd(list_t *lidt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int kernsh_idt_freebsd(list_t *lidt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
