/*                                                                                                                                                          
** stack.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 mayhem                                                                                                                   
*/
#include "elfsh.h"



/* Symbol matching on the stack content. Always useful */
int		vm_dumpstack(uint32_t size, elfsh_Addr start)
{
  long		*i;
  char		logbuf[BUFSIZ];
  char		*name;
  int		off;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" .:: Stack ::.\n");
  
  /* Just a simple loop that dump resolved stack content */
  for (i = (long *) start; i < (long *) start + size; i++)
    {
      if ((elfsh_Addr) i >= E2DBG_KERNELBASE)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot dump anymore : end of stack", -1);

      name = vm_resolve(world.curjob->current, *i, &off);
      if (!name)
	name = "?";
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s + %d> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s> \n", 
		 (elfsh_Addr) i, (elfsh_Addr) *i, name);      
      vm_output(logbuf);
    }
  vm_output("\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Stack display with symbols */
int		cmd_stack()
{
  char		*param;
  elfsh_Addr	  size;
  elfshpath_t	*ssp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));

  param = vm_lookup_var(param);

  ssp = hash_get(&vars_hash, ELFSH_SSPVAR);
  if (!ssp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No saved SP", -1);

  /* Dump debuggee stack */
  if (vm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      return (vm_dumpstack(size, ssp->immed_val.ent));
    }  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Invalid parameter", -1);
}




/* Debugger stack display with symbols */
int		cmd_dbgstack()
{
  char		*param;
  elfsh_Addr	  size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_debug_mode())
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));

  param = vm_lookup_var(param);

  /* Dump debugger stack */
  if (vm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      return (vm_dumpstack(size, (elfsh_Addr) &param));
    }  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Invalid parameter", -1);
}

