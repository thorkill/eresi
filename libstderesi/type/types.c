/**
* @file types.c
* @ingroup libstderesi
* Be able to add your own types in runtime
*
* Started on  Sun Jan 9 07:23:58 2007 jfv
* $Id: types.c,v 1.2 2007-12-06 06:40:16 may Exp $
**
*/
#include "libstderesi.h"



/**
 * Types management command 
 */
int		cmd_type()
{
  int		nocmd;
  unsigned char	isunion;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  isunion = !strcmp(world.curjob->curcmd->name, CMD_UNION);
  nocmd = 0;
  switch (world.curjob->curcmd->argc)
    {
      /* type */
    case 0:
      revm_type_prints();
      break;

      /* type name */
    case 1:
      revm_type_print_regex(world.curjob->curcmd->param[0]);
      revm_output("\n");
      break;

      /* Those case are invalid syntaxes */
    case 2:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid syntax for type command", -1);

    default:
      /* Add a type */
      if (!strcmp(world.curjob->curcmd->param[1], "=") ||
	  !strcmp(world.curjob->curcmd->param[1], "is"))
	{
	  
	  /* Dont forget to filter on command names too, as each type
	     will need to have its name bound on cmd_declare() */
	  if (hash_get(&cmd_hash, world.curjob->curcmd->param[0]))
	    nocmd = 1;
	  if (aspect_type_register(isunion, world.curjob->curcmd->param[0],
				   world.curjob->curcmd->param + 2,
				   world.curjob->curcmd->argc - 2) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid type declaration", -1);
	
	  if (!world.state.revm_quiet)
	    {  
	      revm_output("\n\t [*] Added type ");
	      revm_output(world.curjob->curcmd->param[0]);
	      revm_output(" succesfully \n\n");
	    }

	  /* Create the hash table for objects of that type */
	  revm_type_hashcreate(world.curjob->curcmd->param[0]);
	}
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid syntax for type command", -1);
 
    }

  /* Register this alias as a command */
  if (!nocmd)
    revm_command_add(world.curjob->curcmd->param[0], cmd_declare, 
		     revm_getvarparams, 0, HLP_DECLARE);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Typedef command function 
 */
int  		cmd_typedef()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->argc != 2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid syntax for type command", -1);

  ret = revm_type_copy(world.curjob->curcmd->param[0], 
		     world.curjob->curcmd->param[1]);

  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 (ret == -1 ? "Type not found" : 
		  "Failed to create a new type"), -1);

  /* Register this alias as a command */
  revm_command_add(world.curjob->curcmd->param[1], cmd_declare, 
	    revm_getvarparams, 0, HLP_DECLARE);
  
  /* Output report */
  if (!world.state.revm_quiet)
    {
      revm_output("\n\t [*] Copied type ");
      revm_output(world.curjob->curcmd->param[0]);
      revm_output(" to ");
      revm_output(world.curjob->curcmd->param[1]);
      revm_output(" succesfully \n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
