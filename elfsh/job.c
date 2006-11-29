/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
*/
#include "elfsh.h"

int		cmd_workspace()
{
  elfshjob_t	*job;
  u_int		idx;
  u_int		index;
  hashent_t	*actual;
  char		logbuf[BUFSIZ];
  char		*nl;
  char		*time;
  elfshobj_t	*obj;
  char		**keys;
  int		keynbr;
  char		**loadedkeys;
  int		loadedkeynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  //printf("workspace argc %u \n", world.curjob->curcmd->argc);

  switch (world.curjob->curcmd->argc)
    {
      
      /* $ workspace */
    case 0: 
      vm_output(" .::. Workspaces .::. \n");
      keys = hash_get_keys(&world.jobs, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  actual = hash_get(&world.jobs, keys[index]);
	  job = (elfshjob_t *) actual->data;
	  if (vm_own_job(job))
	    {
	      time = ctime(&job->createtime);
	      nl = strchr(time, '\n');
	      if (nl)
		*nl = 0x00;
	      snprintf(logbuf, BUFSIZ - 1, " [%s] %s %c \n", actual->key, 
		       time, (job->active ? '*' : ' '));
	      vm_output(logbuf);
	      
	      if (hash_size(&job->loaded))
		{
		  loadedkeys = hash_get_keys(&job->loaded, &loadedkeynbr);
		  for (idx = 0; idx < loadedkeynbr; idx++)
		    {
		      obj = hash_get(&job->loaded, loadedkeys[idx]);
		      snprintf(logbuf, BUFSIZ - 1, " \t %c %s \n", 
			       (job->current == obj ? '*' : ' '), obj->name);
		      vm_output(logbuf);
		    }
		  
		}
	      else
		{
		  snprintf(logbuf, BUFSIZ - 1, " \t   No files\n");
		  vm_output(logbuf);
		}
	    }
	}
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* $ workspace name */      
    case 1:
      if (vm_valid_workspace(world.curjob->curcmd->param[0]))
	{
	  job = hash_get(&world.jobs, world.curjob->curcmd->param[0]);
	  if (!job)
	    {
	      /* create a new workspace */
	      job = vm_clone_job(world.curjob);
	      hash_add(&world.jobs, world.curjob->curcmd->param[0], (void *) job);
	    }
	}
      job = hash_get(&world.jobs, world.curjob->curcmd->param[0]); 
      if (vm_own_job(job))
	{
	  /* switch */
	  snprintf(logbuf, BUFSIZ - 1, 
		   "\n [+] Workspace : %s \n\n", world.curjob->curcmd->param[0]);
	  vm_output(logbuf);
	  vm_switch_job(job);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Incorrect workspace name", -1);

      /* Unknown command format */
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong arg number", -1);
    }
}

/* Switch to the next workspace */
int		cmd_next_workspace()
{
  u_int		index;
  char	        **keys;
  int		keynbr;
  elfshjob_t	*curjob;
  elfshjob_t	*next;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  keys = hash_get_keys(&world.jobs, &keynbr);
  if (keynbr <= 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  for (index = 0; index < keynbr; index++)
    {
      curjob = hash_get(&world.jobs, keys[index]);
      if (vm_own_job(curjob) && curjob != world.curjob)
	{
	  next = hash_get(&world.jobs, keys[index + 1 >= keynbr ? 0 : index + 1]);
	  vm_switch_job(next);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	}
    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find workspace to switch on", -1);
}
