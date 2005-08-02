/*
** fcthijack.c for elfsh
** 
** Started on  Thu Jun 19 17:02:55 2003 mayhem
*/
#include "elfsh.h"


/* Redirect a function on a OS independant manner */
int		cmd_hijack()
{
  elfsh_Sym	*dst;
  elfsh_Addr	addr;
  int		err;
  char		*rev;
  char		logbuf[BUFSIZ];
  elfsh_Sym	*sym;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Resolve destination parameter */
  dst = elfsh_get_metasym_by_name(world.curjob->current, world.curjob->curcmd->param[1]);
  if (dst == NULL)
    {
      err = sscanf(world.curjob->curcmd->param[1], XFMT, (elfsh_Addr *) &addr);
      if (err != 1)
	{
	  if (elfsh_dynamic_file(world.curjob->current))
	    {
	      elfsh_setup_hooks();
#if	__DEBUG_HIJACK__
	      printf("[cmd_hijack] trying to add a plt entry (name : %s)\n", 
		     world.curjob->curcmd->param[1]);
#endif
	      if ((elfsh_copy_plt(world.curjob->current, elfsh_get_pagesize(world.curjob->current))) < 0)
		{
#if	__DEBUG_HIJACK__
		      printf("[cmd_hijack] copy_plt failed\n");
#endif
		      return (-1);		  
		}
	      else
		{
		  if ((sym = elfsh_request_pltent(world.curjob->current, world.curjob->curcmd->param[1])))
		    {
#if	__DEBUG_HIJACK__
		      printf("[cmd_hijack] plt entry added (sym->st_value : %08X)\n", sym->st_value);
#endif
		      addr = sym->st_value;
		    }
		  else
		    {
#if	__DEBUG_HIJACK__
		      printf("[cmd_hijack] request_pltent failed\n");
#endif
		      return (-1);		  
		    }
		}
	      

	    }
	  else	  
	  return (-1);
	}
	
      rev = vm_reverse(world.curjob->current, addr);
    }
  else
    {
      addr = dst->st_value;
      rev = NULL;
    }

#if	__DEBUG_HIJACK__
  printf("[cmd_hijack] Resolved %s as %08X \n", world.curjob->curcmd->param[1], addr);
#endif

  /* Hijack function */
  err = elfsh_hijack_function_by_name(world.curjob->current, 
				      ELFSH_HIJACK_TYPE_FLOW,
				      world.curjob->curcmd->param[0], 
				      addr);

  /* Last checks */
  if (err < 0)
    return (-1);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [*] Function %s redirected to addr " XFMT " <%s> \n\n", 
	       world.curjob->curcmd->param[0], (elfsh_Addr) addr, 
	       (rev == NULL ? world.curjob->curcmd->param[1] : rev));
      vm_output(logbuf);
    }
  if (rev != NULL)
    free(rev);
  
  return (0);
}
