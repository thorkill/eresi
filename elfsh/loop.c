/*
** loop.c for  in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Wed Nov 19 23:02:04 2003 mayhem
*/
#include "elfsh.h"


/* Debug purpose */
int		vm_printscript()
{
  elfshargv_t	*list;
  u_int		index;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  for (index = 0, list = world.curjob->script[world.curjob->sourced]; 
       list; list = list->next, index++)
    {
      snprintf(logbuf, BUFSIZ - 1, "[%03u] CMD: %s %p \n",
	       index, list->name, list);
      vm_output(logbuf);
    }
  return (0);
}


/* Scripting report purpose */
void		vm_print_actual(elfshargv_t *cur)
{
  int		idx;
  char		logbuf[BUFSIZ];
    
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1, "~%s ", cur->name);
  vm_output(logbuf);
  for (idx = 0; cur->param[idx] && idx < 10; idx++)
    {
      snprintf(logbuf, BUFSIZ - 1, "%s ", cur->param[idx]);
      vm_output(logbuf);
    }
  putchar('\n');
}



/* Execute the ELFsh script (only used in script mode) */
int		vm_execscript()
{
  elfshargv_t	*cur;
  elfshargv_t	*next;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Curcmd acts like a $PC register */
  for (cur = world.curjob->script[world.curjob->sourced]; cur; cur = next)
    {

      /* Lazy evaluation is only used for module commands called from scripts */
      /* -> We dont have to load the module for the whole script duration */
      if (cur->cmd == NULL)
	{
	  
	  //__asm__(".byte 0xCC");

	  cur->cmd = hash_get(&cmd_hash, cur->param[0]);
	  if (cur->cmd != NULL && cur->cmd->reg != NULL)
	    {
	      ELFSH_CMDARGS_COUNT(cur);
	      if (cur->cmd->reg(0, cur->argc, cur->param) < 0)
		return (vm_doerror(vm_badparam, cur->param[0]));
	    }
	}


      /* Execute instruction */
      if (cur->cmd != NULL && cur->cmd->exec != NULL)
	{
	  if (!world.state.vm_quiet)
	    vm_print_actual(cur);
	  if (vm_implicit(cur->cmd) < 0)
	    ELFSH_SETERROR("Implicit operations failed", -1);
	  if (cur->cmd->exec() < 0)
	    ELFSH_SETERROR("Command execution failed", -1);
	}
      else
	 vm_error("Lazy evaluation failed for command", cur->param[0]);

      /* Current instruction modified the script flow ? */
      if (cur == world.curjob->curcmd)
	{
	  next = cur->next;
	  world.curjob->curcmd = next;
	}
      else
	next = world.curjob->curcmd;

    }
  return (0);
}



/* Execute the ELFsh current command (only used in interactive mode) */
int		vm_execmd()
{
  elfshargv_t	*cur;
  elfshargv_t	*next;
  int		err;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Curcmd acts like a $PC register */
  for (err = 0, cur = world.curjob->script[world.curjob->sourced]; cur; 
       world.curjob->curcmd = cur = cur->next)
    if (cur->cmd != NULL && cur->cmd->exec != NULL)
      {
	if (vm_implicit(cur->cmd) < 0)
	  {
	    err = -1;
	    goto end;
	  }
	if (cur->cmd->exec() < 0)
	  {
	    err = -1;
	    goto end;
	  }
      }
  
  /* Free the chain. We can do that in interactive mode 
   * because there is no loop support. 
   */
 end:
  for (cur = world.curjob->script[world.curjob->sourced]; cur; cur = next)
    {
      next = cur->next;
      //      free(cur); 
    }
  world.curjob->script[world.curjob->sourced] = world.curjob->curcmd = NULL;
  world.curjob->lstcmd[world.curjob->sourced] = NULL;
  return (err);
}



/* Take the ELF machine control flow in charge */
int		vm_move_pc(char *param)
{
  int		index;
  int		jmp;
  elfshargv_t	*next;
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* check if we match a label */
  next = hash_get(&labels_hash[world.curjob->sourced], param);
  if (next)
    {
      if (!next->cmd)
	ELFSH_SETERROR("Invalid EOF branchement", -1);
      world.curjob->curcmd = next;
      return (0);
    }
  
  /* Use the parameter as a numerical index */
  jmp = atoi(param);
  if (jmp < 0)
    for (index = 0, jmp = -jmp; index != jmp; index++)
      {
	if (!world.curjob->curcmd->prev)
	  ELFSH_SETERROR("Invalid backward branchement", -1);
	world.curjob->curcmd = world.curjob->curcmd->prev;
      }
  else if (jmp > 0)
    for (index = 0; index != jmp; index++)
      {
	if (!world.curjob->curcmd->next)
	  ELFSH_SETERROR("Invalid forward branchement", -1);
	world.curjob->curcmd = world.curjob->curcmd->next;
      }
  else
    ELFSH_SETERROR("Invalid null branchement", -1);
  
  return (0);
}












