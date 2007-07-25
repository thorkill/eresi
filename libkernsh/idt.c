/*
** idt.c for libkernsh
**
** $Id: idt.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "libkernsh.h"

int kernsh_idt()
{
  int ret;
  u_int         dim[3];
  vector_t      *idt;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  
  idt = aspect_vector_get("idt");
  dim[0] = libkernshworld.type;
  dim[1] = libkernshworld.os;

  fct = aspect_vectors_select(idt, dim);

  ret = fct();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


int kernsh_idt_linux()
{
  int i;
  char buff[512];
  libkernshint_t dint;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf("IDT LINUX\n");

  if (kernsh_is_static_mode())
    {
      //elfsh_reverse_metasym_by_name
    }
  else 
    {
      if (!libkernshworld.open)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Memory not open !", -1);
	}

      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s\n\n",
	       revm_colorfieldstr("[+] IDT"));
      revm_output(buff);

      for (i = 0;
	   i < (libkernshworld.idt_limit + 1) / (sizeof(unsigned long) * 2); 
	   i++)
	{
	  memset(&dint, '\0', sizeof(dint));
	  kernsh_readmem(libkernshworld.idt_base+sizeof(unsigned long)*2*i,
			 &idt,
			 sizeof(idt));

	  dint.addr = (unsigned long)(idt.off2 << 16) + idt.off1;
	  kernsh_resolve_systemmap(dint.addr, dint.name, sizeof(dint.name));

          memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff),
		   "%s %-40s %s %s\n",
		   revm_colornumber("id:%-10u", (unsigned int)i),
		   revm_colortypestr_fmt("%s", dint.name),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) dint.addr));
	  revm_output(buff);
	  revm_endline();
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}

