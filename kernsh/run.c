/*
** run.c for kernsh
** 
** $Id: run.c,v 1.1 2007-07-25 19:53:01 pouik Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"


/* Open the memory device */
int		cmd_openmem()
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  /* Check if no static is set */
  if (!(int) config_get_data(LIBKERNSH_VMCONFIG_NOSTATIC))
    {
      // DECOMP THE KERNEL !!
      // ret = kernsh_decompkernel();
      // LOAD IT !!!
      ret = kernsh_loadkernel();

      if (ret)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to load kernel", -1);
    }
  else 
    {
      kernsh_set_mem_mode();
    }

  ret = kernsh_openmem();

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot open memory", -1);

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Close the memory device */
int		cmd_closemem()
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = kernsh_unloadkernel();
    
  ret = kernsh_closemem();

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot close memory", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/*  */
int		cmd_kmode()
{
  char          *param;
  char		buf[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if (*param == 's' || *param == 'S')
	{
	  if (!libkernshworld.open_static)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Static kernel is not loaded !", -1);
	  
	  kernsh_set_static_mode();
	  printf(" [*] kernsh is now in STATIC mode\n\n");
	}
      else if (*param == 'd' || *param == 'D')
	{
	  if (!libkernshworld.open)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unable to switch to dynamic mode, please open the memory !", -1);
	  
	  kernsh_set_mem_mode();
	  printf(" [*] kernsh is now in DYNAMIC mode\n\n");
	}
      
      else
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Unknown mode for kernsh !", -1);
	}
    }
  else
    {
      if (kernsh_is_static_mode())
	param = "STATIC";
      
      else if (kernsh_is_mem_mode())
	param = "DYNAMIC";
      
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unknown kernsh mode", -1);
      
      snprintf(buf, sizeof(buf), " [*] kernsh is in %s MODE \n\n", param);
	  printf("%s", buf);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }


#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Display the sys_call_table */
int		cmd_sct()
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = kernsh_sct();

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get syscalltable", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/*Display the idt */
int		cmd_idt()
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ret = kernsh_idt();

#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get idt", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Manipulate memory device */
int		cmd_mem()
{
  char          *param, *param2;
  char		buff[256];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  param = world.curjob->curcmd->param[0];
  param2 = world.curjob->curcmd->param[1];

  if (param)
    {
      if (param2 && !strcmp(param, "alloc"))
	{
	  if(kernsh_alloc_contiguous(atoi(param2), &addr))
	    {
	    }
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff), 
		   "%s %s %s %s %s %s\n\n",
		   revm_colorfieldstr("ALLOCATE"),
		   revm_colornumber("%u", (unsigned int)atoi(param2)),
		   revm_colorfieldstr("octet(s)"),
		   revm_colorfieldstr("OF CONTIGUOUS MEMORY"),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) addr));
	  revm_output(buff);
	    
	}
      else if (param2 && !strcmp(param, "free"))
	{
	  addr = strtoul( param2, NULL, 16 );
	  if(kernsh_free_contiguous(addr))
	    {
	    }
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff), 
		   "%s %s %s %s\n\n",
		   revm_colorfieldstr("FREE"),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) addr),
		   revm_colorfieldstr("OF CONTIGUOUS MEMORY"));
	  revm_output(buff);
	}
      else if (param2 && !strcmp(param, "alloc_nc"))
	{
	  if(kernsh_alloc_noncontiguous(atoi(param2), &addr))
	    {
	    }
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff), 
		   "%s %s %s %s %s %s\n\n",
		   revm_colorfieldstr("ALLOCATE"),
		   revm_colornumber("%u", (unsigned int)atoi(param2)),
		   revm_colorfieldstr("octet(s)"),
		   revm_colorfieldstr("OF NON CONTIGUOUS MEMORY"),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) addr));
	  revm_output(buff);
	}
      else if (param2 && !strcmp(param, "free_nc"))
	{
	  addr = strtoul( param2, NULL, 16 );
	  if(kernsh_free_noncontiguous(addr))
	    {
	    }
	  memset(buff, '\0', sizeof(buff));
	  snprintf(buff, sizeof(buff), 
		   "%s %s %s %s\n\n",
		   revm_colorfieldstr("FREE"),
		   revm_colorstr("@"),
		   revm_coloraddress(XFMT, (elfsh_Addr) addr),
		   revm_colorfieldstr("OF NONCONTIGUOUS MEMORY"));
	  revm_output(buff);
	}
      else 
	{

	}
    }
#if defined(USE_READLN)
  rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
  readln_column_update();
#endif
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
