/*
** @file implicit.c
** 
** Started on  Tue Feb 18 06:45:35 2003 jfv
**
** $Id: implicit.c,v 1.6 2007-11-29 14:01:55 may Exp $
**
*/
#include "revm.h"



/* Load the working files */
void				revm_workfiles_load()
{
  char				logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.input == NULL)
    {
      revm_help(NULL);
      revm_exit(-1);
    }
  world.curjob->current = (world.state.output != NULL ? 
			   elfsh_map_obj(world.state.input) :
			   elfsh_load_obj(world.state.input));
  if (world.curjob->current == NULL)				
    {								
      perror(world.state.input);					
      revm_exit(-1);						
    }					

  hash_add(&world.curjob->loaded, world.curjob->current->name,
	   world.curjob->current);
  hash_add(&file_hash, world.curjob->current->name, 
	   (void *) world.curjob->current);

  if (!world.state.revm_quiet)	
    {
      snprintf(logbuf, BUFSIZ - 1, "\n [*] Object %s has been loaded (%s) \n\n", 
	       world.state.input, 
	       (world.state.output != NULL ? "O_RDWR" : "O_RDONLY"));
      revm_output(logbuf);
    }

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}




/* Unload (and save if asked) files */
int				revm_workfiles_unload()
{
  char				logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.output)
    switch (elfsh_save_obj(world.curjob->current, world.state.output))
      {
      case 0:
	if (!world.state.revm_quiet)
	  {
	    snprintf(logbuf, BUFSIZ - 1,
		     " [*] Object %s saved successfully \n\n",
		     world.state.output);
	    revm_output(logbuf);
	  }
	break;
      default:
	snprintf(logbuf, BUFSIZ - 1,
		 " [*] Unable to save modified object in %s \n\n", 
		 world.state.output);
	revm_output(logbuf);
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Failed to save object", (-1));
      }
  else
    {
      elfsh_unload_obj(world.curjob->current);
      if (!world.state.revm_quiet)
	{
	  snprintf(logbuf, BUFSIZ - 1,
		   " [*] Object %s unloaded \n\n", world.state.input);
	  revm_output(logbuf);
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Do all implicit operation when calling elfsh from a script */
int		revm_implicit(revmcmd_t *actual)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If the requested command does not need a current pointer */
  if (actual && !actual->wflags)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Load implicit current file */
  else if (world.state.revm_mode == REVM_STATE_CMDLINE && 
	   world.curjob->current == NULL)
    {
      if (!world.state.revm_sourcing)
	revm_workfiles_load();
      if (!world.curjob->current)
	{
	  revm_help(NULL);
	  revm_exit(-1);
	}
    }
  
  /* Do error in imode if current is NULL */
  else if (((world.state.revm_mode == REVM_STATE_INTERACTIVE ||
	     world.state.revm_mode == REVM_STATE_SCRIPT) && 
	    !world.curjob->current))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "No file loaded", -1);

  /* We need to set it here since the CURRENT object can change */
  asm_set_resolve_handler(&world.proc, asm_do_resolve, world.curjob->current);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
















