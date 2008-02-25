/*
** open.c for kernsh
** 
** $Id: open.c,v 1.10 2007-11-29 15:33:39 may Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

int		export_var(char *name, 
			   unsigned long vallong, 
			   int valint, 
			   char *valstr,
			   int choice)
{
  char			param1[BUFSIZ];
  char			param2[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(param1, '\0', sizeof(param1));
  memset(param1, '\0', sizeof(param2));

  snprintf(param1,
	   sizeof(param1),
	   "$%s",
	   name);

  switch (choice)
    {
    case 0 :
      snprintf(param2,
	       sizeof(param2),
	       "%d",
	       valint);
      break;

    case 1 :
      snprintf(param2,
	       sizeof(param2),
	       "0x%lx",
	       vallong);
      break;
    
    case 2 :
      snprintf(param2,
	       sizeof(param2),
	       "%s",
	       valstr);
      break;

    default :
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid choice", (-1));
    }
  
  world.curjob->curcmd->param[0] = param1;
  world.curjob->curcmd->param[1] = param2;

  cmd_set();
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
 
int		export_vars()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __KERNSH_DEBUG__
  printf("EXPORT VARS\n");
#endif

  export_var("sct", libkernshworld.sct, 0, NULL, 1);
  export_var("idt_base", libkernshworld.idt_base, 0, NULL, 1);
  export_var("idt_limit", 0, libkernshworld.idt_limit, NULL, 0);
  export_var("gdt_base", libkernshworld.gdt_base, 0, NULL, 1);
  export_var("gdt_limit", 0, libkernshworld.gdt_limit, NULL, 0);
  
#if defined (__linux__)
  export_var("system_call", libkernshworld.system_call, 0, NULL, 1);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Open the memory device */
int		cmd_openmem()
{
  int		ret;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if no static is set */
  if (!(int) config_get_data(LIBKERNSH_VMCONFIG_WITHOUT_KERNEL))
    {
      if(!(int) config_get_data(LIBKERNSH_VMCONFIG_USE_KERNEL))
	{
	  /* Gunzip/Extract the kernel */
	  ret = kernsh_decompkernel();
	}
      
      memset(buff, '\0', sizeof(buff));
      snprintf(buff,
	       sizeof(buff),
	       "%s%s",
	       (char *) config_get_data(LIBKERNSH_VMCONFIG_STORAGE_PATH),
	       (char *) config_get_data(LIBKERNSH_VMCONFIG_KERNELELF));
      
      ret = revm_file_load(buff, 0, NULL);

      if (ret)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to load kernel", -1);
    
      libkernshworld.root = revm_lookup_file(buff);
      if (libkernshworld.root == NULL)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unable to get elf file", -1);
	}
      libkernshworld.open_static = 1;
    }
  else 
    {
      kernsh_set_mem_mode();
    }

  /* Open memory */
  ret = kernsh_openmem();

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot open memory", -1);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] OPEN MEMORY"));
  revm_output(buff);
  revm_endline();
  export_vars();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
