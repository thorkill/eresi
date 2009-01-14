/*
** stack.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 jfv                                                                                                                   
**
** $Id$
**
*/
#include "libe2dbg.h"



/* Symbol matching on the stack content. Always useful */
int		e2dbg_stack_dump(uint32_t size, eresi_Addr start)
{
  long		*i;
  char		logbuf[BUFSIZ];
  char		*name;
  elfsh_SAddr	off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  e2dbg_output(" .:: Stack ::.\n");
  
  /* Just a simple loop that dump resolved stack content */
  for (i = (long *) start; i < (long *) start + size; i++)
    {
      if ((eresi_Addr) i >= E2DBG_KERNELBASE)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot dump anymore : end of stack", -1);

      name = revm_resolve(world.curjob->curfile, *i, &off);
      if (!name)
	name = "?";
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s + " DFMT "> \n", 
		 (eresi_Addr) i, (eresi_Addr) *i, name, off);
      else
	snprintf(logbuf, BUFSIZ - 1, " " XFMT " " XFMT " <%s> \n", 
		 (eresi_Addr) i, (eresi_Addr) *i, name);      
      e2dbg_output(logbuf);
    }
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Stack display with symbols */
int		cmd_stack()
{
  char		*param;
  eresi_Addr	  size;
  revmobj_t	*ssp;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_runtime_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));

  param = revm_lookup_string(param);
  expr = revm_expr_get(E2DBG_SSP_VAR);
  if (!expr || !expr->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "No saved SP", -1);
  ssp = expr->value;

  /* Dump debuggee stack */
  if (revm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 e2dbg_stack_dump(size, ssp->immed_val.ent));
    }  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Invalid parameter", -1);
}




/* Debugger stack display with symbols */
int		cmd_dbgstack()
{
  char		*param;
  eresi_Addr	  size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!elfsh_is_runtime_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Command only available in e2dbg", (-1));
  param = world.curjob->curcmd->param[0];
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (-1));

  param = revm_lookup_string(param);

  /* Dump debugger stack */
  if (revm_isnbr(param))
    {
      if (sscanf(param, UFMT, &size) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid argument", (-1));
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
			 e2dbg_stack_dump(size, (eresi_Addr) &param));
    }  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Invalid parameter", -1);
}

