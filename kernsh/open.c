/*
** open.c for kernsh
** 
** $Id: open.c,v 1.8 2007-10-11 18:25:17 pouik Exp $
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
  
  revmobj_t             *o1;
  revmobj_t             *o2;
  int                   error;
  int                   errvar;
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

  error  = -1;
  errvar = 0;
  if (revm_preconds_atomics(&o1, &o2) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Invalid variable transaction", (-1));
  errvar = revm_object_set(o1, o2);
  if (errvar < 0)
    {
      if (errvar != -2)
        errvar = 0;
      goto err;
    }
 err:
  if (!o2->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o2);
  if (!o1->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__, o1);

  /* We have 2 different possible errors here */
  if (errvar < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Error while setting result variable", -1);
  else if (error < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to set object", -1);

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

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

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

#if defined(USE_READLN)
  rl_callback_handler_install(revm_get_prompt(), revm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
