/**
* @file libstderesi/analysis/argcount.c
* @ingroup analysis
* @brief Command argcount (count arguments in a function)
*
* Started Jul 2 2005 00:03:44 mxatone
* $Id: trace.c 1397 2009-09-13 02:19:08Z may $
*
*/
#include "libstderesi.h"


int		cmd_argcount()
{
  elfsh_Sym	*sym;
  eresi_Addr	addr;
  int		idx;
  int		*args;
  char		*name;
  int		off;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Resolve parameter */
  sym = elfsh_get_metasym_by_name(world.curjob->curfile,
				  world.curjob->curcmd->param[0]);
  if (sym)
    {
      addr = sym->st_value;
      name = world.curjob->curcmd->param[0];
    }
  else
    {
      addr = revm_lookup_addr(world.curjob->curcmd->param[0]);
      if (addr == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid function/address request", -1);
      name = elfsh_reverse_metasym(world.curjob->curfile, addr, &off);
      if (!name)
	name = "func-unresolved";
    }

  /* Call feature vector */
  args = elfsh_args_count(world.curjob->curfile, 0, addr);
  if (args == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to count arguments", -1);
  snprintf(buf, sizeof(buf), " [*] Argument counting on function <%s> ("XFMT")\n",
	   name, addr);
  for (idx = 0; args[idx]; idx++)
    {
      snprintf(buf, sizeof(buf), "  VAR %u - of size %d bytes \n", idx, args[idx]);
      revm_output(buf);
    }
  revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
