/*
** loop.c for elfsh
** 
** This file include the control flow support for scripting
**
** Started on  Wed Nov 19 23:02:04 2003 mayhem
** Updated on  Mon Aug 15 06:01:54 2005 mayhem
*/
#include "revm.h"


/* Debug purpose */
int		vm_printscript(revmargv_t *start)
{
  revmargv_t	*list;
  u_int		index;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output("Printing Script ! \n");

  if (start)
    list = start;
  else
    list = world.curjob->script[world.curjob->sourced];

  for (index = 0; list; list = list->next, index++)
    {
      snprintf(logbuf, BUFSIZ - 1, "[%03u] CMD: %s %p \n",
	       index, list->name, list);
      vm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Scripting report purpose */
void		vm_print_actual(revmargv_t *cur)
{
  int		idx;
  char		logbuf[BUFSIZ];
    
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1, "~%s ", cur->name);
  vm_output(logbuf);
  for (idx = 0; cur->param[idx] && idx < 10; idx++)
    {
      snprintf(logbuf, BUFSIZ - 1, "%s ", cur->param[idx]);
      vm_output(logbuf);
    }
  putchar('\n');
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Execute the script (only used in script mode) */
/* Script mode include sourced scripts in elfsh or e2dbg */
int		vm_execscript()
{
  revmargv_t	*cur;
  revmargv_t	*next;
  int		status;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Restore the command pointer if we used 'cont' */
  if (world.state.vm_sourcing)
    {
      cur = world.context.curcmd;
      world.curjob->curcmd = cur;
      printf("restored command context after CONT ! \n");
    }
  else
    cur = world.curjob->script[world.curjob->sourced];

  /* Curcmd acts like a $PC register */
  for (; cur; cur = next)
    {

      /* Lazy evaluation is only used for module commands called from scripts */
      /* -> We dont have to load the module for the whole script duration */
      if (cur->cmd == NULL)
	{
	  cur->cmd = hash_get(&cmd_hash, cur->param[0]);
	  if (cur->cmd != NULL && cur->cmd->reg != NULL)
	    {
	      REVM_CMDARGS_COUNT(cur);
	      if (cur->cmd->reg(0, cur->argc, cur->param) < 0)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Commande parsing failed",
				  vm_doerror(vm_badparam, cur->param[0]));
	    }
	}

      /* Test for e2dbg scripting */
      if (!strcmp(cur->name, CMD_CONTINUE) || !strcmp(cur->name, CMD_CONTINUE2))
	{
	  next                    = cur->next;
	  world.context.curcmd    = next;
	  world.state.vm_sourcing = 1;
	  printf("Found continue in script, saving context & sourcing flag \n");
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     E2DBG_SCRIPT_CONTINUE);
	}

      /* Execute instruction */
      if (cur->cmd != NULL && cur->cmd->exec != NULL)
	{
	  if (!world.state.vm_quiet)
	    vm_print_actual(cur);
	  if (vm_implicit(cur->cmd) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Implicit operations failed", -1);
	  status = cur->cmd->exec();
	  if (status < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Command execution failed", -1);
	}
      else
	 vm_error("Lazy evaluation failed for command", cur->param[0]);

      /* Current instruction modified the script control flow ? */
      if (cur == world.curjob->curcmd)
	{
	  next = cur->next;
	  world.curjob->curcmd = next;
	}
      else
	next = world.curjob->curcmd;

      /* Break the flow if we switched to interactive mode */
      if (status == ELFSH_SCRIPT_STOP)
	break;
    }

  /* If we had a saved context, restore it */
  if (world.state.vm_sourcing)
    {
      world.curjob->lstcmd[world.curjob->sourced] = NULL;
      vm_restore_dbgcontext(world.context.savedfd,
			    world.context.savedmode,
			    world.context.savedcmd,
			    world.context.savedinput,
			    world.context.savedargv,
			    world.context.savedname);
      world.curjob->curcmd = NULL;
      world.state.vm_sourcing = 0;
      printf("Restoring e2dbg context from sourced script \n");
    }
  
  /* Make sure we switch to interactive mode if we issued a stop command */
  if (status == ELFSH_SCRIPT_STOP)
    world.state.vm_mode = ELFSH_VMSTATE_IMODE;
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, status);
}



/* Execute the ELFsh current command (only used in interactive mode) */
int		vm_execmd()
{
  //revmargv_t	*next;
  revmjob_t	*curjob;
  revmargv_t	*cur;
  int		err;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  curjob = world.curjob;

  /* Curcmd acts like a $PC register */
  for (err = 0, curjob->curcmd = cur = curjob->script[curjob->sourced]; cur; 
       curjob->curcmd = cur = cur->next)
    if (cur->cmd != NULL && cur->cmd->exec != NULL)
      {
	if (vm_implicit(cur->cmd) < 0)
	  {
	    profiler_error();
	    err = -1;
	    goto end;
	  }

	/* Execute the command exec handler */
	ret = cur->cmd->exec();
	
	/* Problem during the command */
	if (ret < 0)
	  {
	    err = -1;
	    goto end;
	  }

	/* We are executing 'cont' from e2dbg */
	else if (ret == E2DBG_SCRIPT_CONTINUE)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     E2DBG_SCRIPT_CONTINUE);
      }
  

   end:
  
  /* 
  ** Free the chain. We can do that in interactive mode 
  ** because there is no loop support
  **
  for (cur = world.curjob->script[world.curjob->sourced]; cur; cur = next)
  {
  next = cur->next;
  XFREE(__FILE__, __FUNCTION__, __LINE__,cur); 
  }
  */
  
  curjob->script[curjob->sourced] = curjob->curcmd = NULL;
  curjob->lstcmd[curjob->sourced] = NULL;

  if (err < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, err);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Take the ELF machine control flow in charge */
int		vm_move_pc(char *param)
{
  int		index;
  int		jmp;
  revmargv_t	*next;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* check if we match a label */
  next = hash_get(&labels_hash[world.curjob->sourced], param);
  if (next)
    {
      if (!next->cmd)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid EOF branchement", -1);
      world.curjob->curcmd = next;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Use the parameter as a numerical index */
  jmp = atoi(param);
  if (jmp < 0)
    for (index = 0, jmp = -jmp; index != jmp; index++)
      {
	if (!world.curjob->curcmd->prev)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Invalid backward branchement", -1);
	world.curjob->curcmd = world.curjob->curcmd->prev;
      }
  else if (jmp > 0)
    for (index = 0; index != jmp; index++)
      {
	if (!world.curjob->curcmd->next)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Invalid forward branchement", -1);
	world.curjob->curcmd = world.curjob->curcmd->next;
      }
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid null branchement", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}












