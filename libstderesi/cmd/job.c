/*
** @file job.c
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
**
** $Id: job.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"

/**
 * Need doxygen comment
 */
int		cmd_workspace()
{
  revmjob_t	*job;
  u_int		idx;
  u_int		index;
  u_short	new = 0;
  char		logbuf[BUFSIZ];
  char		*nl;
  char		*time;
  elfshobj_t	*obj;
  char		**keys;
  int		keynbr;
  char		**loadedkeys;
  int		loadedkeynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  //printf("workspace argc %u \n", world.curjob->curcmd->argc);

  switch (world.curjob->curcmd->argc)
    {
      
      /* $ workspace */
    case 0: 
      revm_output(" .::. Workspaces .::. \n");
      keys = hash_get_keys(&world.jobs, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  job = (revmjob_t *) hash_get(&world.jobs, keys[index]);
	  if (revm_own_job(job))
	    {
	      time = ctime(&job->ws.createtime);
	      nl = strchr(time, '\n');
	      if (nl)
		*nl = 0x00;
	      snprintf(logbuf, BUFSIZ - 1, " [%s] %s %c \n", keys[index], 
		       time, (job->ws.active ? '*' : ' '));
	      revm_output(logbuf);
	      
	      if (hash_size(&job->loaded))
		{
		  loadedkeys = hash_get_keys(&job->loaded, &loadedkeynbr);
		  for (idx = 0; idx < loadedkeynbr; idx++)
		    {
		      obj = hash_get(&job->loaded, loadedkeys[idx]);
		      snprintf(logbuf, BUFSIZ - 1, " \t %c %s \n", 
			       (job->curfile == obj ? '*' : ' '), obj->name);
		      revm_output(logbuf);
		    }
		  
		}
	      else
		{
		  snprintf(logbuf, BUFSIZ - 1, " \t   No files\n");
		  revm_output(logbuf);
		}
	    }
	}
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* $ workspace name */      
    case 1:
      if (!revm_valid_workspace(world.curjob->curcmd->param[0]))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Incorrect workspace name", -1);

      job = hash_get(&world.jobs, world.curjob->curcmd->param[0]);
      if (!job)
	{
	  /* create a new workspace */
	  job = revm_clone_job(strdup(world.curjob->curcmd->param[0]),
 
			     world.curjob);
	  hash_add(&world.jobs, world.curjob->curcmd->param[0], (void *) job);
	  new = 1;
	}
      else
	{
	  if (job->ws.active)
	    {
	      snprintf(logbuf, BUFSIZ - 1,
		       "\n [+] Already in workspace : %s\n\n", world.curjob->curcmd->param[0]);
	      revm_output(logbuf);

	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	    }
	}

      if (revm_own_job(job))
	{
	  /* switch */
	  snprintf(logbuf, BUFSIZ - 1, 
		   "\n [+] Workspace : %s \n\n", world.curjob->curcmd->param[0]);
	  revm_output(logbuf);
	  revm_prompt_log();

	  /* Switch to the new job */
	  revm_switch_job(job);

	  /* Update the screen */
	  revm_screen_update(new, 0);

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Incorrect workspace name", -1);

      /* Unknown command format */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong arg number", -1);
    }
}

/** 
 * Switch to the next workspace 
 */
int		cmd_next_workspace()
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  ret = revm_workspace_next();
  if (!ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
	       "Unable to switch workspace", -1);
}
