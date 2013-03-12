/**
* @file librevm/vm/loop.c
 * @ingroup vm
 * @brief This file include the control flow support for scripting.
 *
 * Started on  Wed Nov 19 23:02:04 2003 jfv
 * $Id$
 */
#include "revm.h"


/** 
 * The infinite main loop of the interactive command
 */
int		revm_loop(int argc, char **argv)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = 0;

  do {

    /* Come back to the loop if a script turns into interactive mode */
  reenter:

    /* Fill argv from stdin if we are in interactive mode */
    if ((world.state.revm_mode != REVM_STATE_CMDLINE 
	 && world.state.revm_mode != REVM_STATE_TRACER) 
	|| world.state.revm_net == 1)
      {
	if (world.state.revm_mode != REVM_STATE_SCRIPT)
	  {
	    if (revm_select() < 0)
	      {
		fprintf(stderr,"revm_select : failed \n");
		revm_exit(-1);
	      }

	    /* If the FIFO does not exist anymore, 
	       the server has quit, so we quit too */
	    if (world.state.revm_mode == REVM_STATE_EMBEDDED && 
		(access(REVM_FIFO_S2C, F_OK) < 0 || 
		 access(REVM_FIFO_C2S, F_OK) < 0))
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);	      
	  }

	/* Take a line, execute old command if void line */
	argv = revm_input(&argc, NULL);
	if (world.state.revm_mode == REVM_STATE_INTERACTIVE ||
	    world.state.revm_mode == REVM_STATE_EMBEDDED    ||
	    world.state.revm_mode == REVM_STATE_SCRIPT      ||
	    world.state.revm_net)
	  {
	    if (argv == ((char **) REVM_INPUT_VOID))
	      continue;
	    else if (argv == ((char **) REVM_INPUT_TRANSFERED))
	      continue;
	  }

	/* CTRL-D -> !argv */
	if (!argv)
	  {
	    revm_output("\n");

	    /* when debugging -> back to main program */
	    if (world.state.revm_mode == REVM_STATE_EMBEDDED)
	      {
		revm_callback_handler_remove();
		revm_cleanup();
		PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
	      }

	    /* if net is enable but we are not in e2dbg -> ignore */
	    if (world.state.revm_net)
	      continue;

	    /* othewise exit */
	    break;

	  }
      }

    /* Fetch the current scripting command */
    if (revm_parseopt(argc, argv) < 0)
      {
	if (world.state.revm_mode != REVM_STATE_CMDLINE
	    && world.state.revm_mode != REVM_STATE_TRACER)
	  {
	    XFREE(__FILE__, __FUNCTION__, __LINE__, argv);
	    if (world.state.revm_mode != REVM_STATE_INTERACTIVE &&
		world.state.revm_mode != REVM_STATE_EMBEDDED)
	      goto end;
	  }
	else if (!world.state.revm_net)
	  revm_exit(-1);
      }

    /* Just execute one command if we are not in script mode */
    if (world.state.revm_mode != REVM_STATE_SCRIPT)
      {
	world.curjob->curcmd = world.curjob->recur[0].script;
	switch (revm_execmd())
	  {
	  case REVM_SCRIPT_CONTINUE:
	    //printf(" [*] e2dbg continue from revm_execmd \n");
	    revm_callback_handler_remove();
	    revm_cleanup();
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);		
	  case REVM_SCRIPT_ERROR:
	    profiler_error();
	  default:
	    break;
	  }
      }

    /* Quit parsing if necessary */
    if ((!world.curjob->curcmd && world.state.revm_mode == REVM_STATE_SCRIPT))
      break;
  }
  while ((world.state.revm_mode != REVM_STATE_CMDLINE
	  && world.state.revm_mode != REVM_STATE_TRACER)
	 || world.state.revm_net);

  /* If we are in scripting, execute commands list now */
  if (world.state.revm_mode == REVM_STATE_SCRIPT)
    {
      fprintf(stderr, "we are in script mode from revm_loop ! \n");

      world.curjob->curcmd = world.curjob->recur[0].script;
      ret = revm_execscript();
      if (ret == REVM_SCRIPT_STOP)
	{
	  XCLOSE(world.curjob->ws.io.input_fd, -1);
	  world.curjob->ws.io.input_fd = 0;
	  goto reenter;
	}
      else if (ret < 0)
	profiler_error();
    }

 end:
  revm_postexec(ret);
  revm_callback_handler_remove();
  revm_cleanup();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/**
 * Execute an ERESI script (only used in non-interactive mode or when sourcing a script)
 */
int		revm_execscript()
{
  revmargv_t	*cur;
  revmargv_t	*next;
  int		status;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Restore the command pointer if we used 'cont' */
  if (world.state.revm_sourcing)
    {
      cur = world.context.curcmd;
      world.curjob->curcmd = cur;
    }
  else
    cur = world.curjob->recur[world.curjob->curscope].script;

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
		{
		  revm_cleanup();
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			       "Commande parsing failed",
			       revm_doerror(revm_badparam, cur->param[0]));
		}
	    }
	}

      /* Execute instruction */
      if (cur->cmd != NULL && cur->cmd->exec != NULL)
	{
	  if (!world.state.revm_quiet)
	    revm_print_actual(cur);
	  if (revm_implicit(cur->cmd) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Implicit operations failed", -1);
	  status = cur->cmd->exec();
	  if (status < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Command execution failed", -1);

	  /* Test for (embedded or not) debugger scripting */
	  if ((world.curjob->curfile && world.curjob->curfile->iotype != ELFSH_HOST_GDB) && 
	      (!strcmp(cur->name, CMD_CONTINUE)  || !strcmp(cur->name, CMD_CONTINUE2) ||
	       !strcmp(cur->name, CMD_CONTINUE3)))
	    {
	      next                    = cur->next;
	      world.context.curcmd    = next;
	      world.state.revm_sourcing = 1;
	      fprintf(stderr, "Found -continue- in script, sourcing flag now -ON- \n");
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, REVM_SCRIPT_CONTINUE);
	    }

	}
      else
	 revm_error("Unable to find command", cur->param[0]);

      /* Current instruction modified the script control flow ? */
      if (cur == world.curjob->curcmd)
	{
	  next = cur->next;
	  world.curjob->curcmd = next;
	}
      else
	next = world.curjob->curcmd;

      /* Break the flow if we switched to interactive mode */
      switch (status)
	{
	case REVM_SCRIPT_STOP:
	case REVM_SCRIPT_QUIT:
	  world.context.curcmd    = next;
	  goto end;
	case REVM_SCRIPT_CONTINUE:
	  world.context.curcmd    = next;
	  world.state.revm_sourcing = 1;
	  fprintf(stderr, "Found -start- in script, sourcing flag now -ON- \n");
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			REVM_SCRIPT_CONTINUE);
	}
    }


  /* We finished executing the script for now */
 end:

  /* If we had a saved context, restore it */
  if (world.state.revm_sourcing)
    {
      world.curjob->recur[world.curjob->curscope].lstcmd = NULL;
      revm_context_restore(world.context.savedfd,
			   world.context.savedmode,
			   world.context.savedcmd,
			   world.context.savedinput,
			   world.context.savedargv,
			   world.context.savedname);
      world.curjob->curcmd = NULL;
      world.state.revm_sourcing = 0;
    }
  
  /* Make sure we switch to interactive mode if we issued a stop command */
  if (status == REVM_SCRIPT_STOP)
    world.state.revm_mode = REVM_STATE_INTERACTIVE;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, status);
}



/** 
 * Execute the current script command
 */
int		revm_execmd()
{
  revmjob_t	*curjob;
  revmargv_t	*cur;
  int		err;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  curjob = world.curjob;

  /* Curcmd acts like a $PC register */
  for (err = 0, curjob->curcmd = cur = curjob->recur[curjob->curscope].script; cur; 
       curjob->curcmd = cur = cur->next)
    if (cur->cmd != NULL && cur->cmd->exec != NULL)
      {
	/* Reset profiler message before anything */
	profiler_error_reset();

	if (revm_implicit(cur->cmd) < 0)
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
	else if (ret == REVM_SCRIPT_CONTINUE)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			REVM_SCRIPT_CONTINUE);
      }
  

   end:
  
  curjob->recur[curjob->curscope].script = curjob->curcmd = NULL;
  curjob->recur[curjob->curscope].lstcmd = NULL;
  if (err < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, err);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/** 
 * Take the ERESI machine control flow in charge 
 */
int		revm_move_pc(char *param)
{
  int		index;
  int		jmp;
  revmargv_t	*next;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* check if we match a label */
  next = hash_get(&world.curjob->recur[world.curjob->curscope].labels, param);
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
