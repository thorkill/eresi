/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
*/
#include "elfsh.h"




elfshjob_t	*vm_clone_job(elfshjob_t      *job)
{
  elfshjob_t    *new;
  int		i;

  XALLOC(new, sizeof(elfshjob_t), NULL);

  memcpy(new, job, sizeof(elfshjob_t));
  
  /* empt?? new job */
  new->list = NULL;
  new->current = NULL;
  new->curcmd = NULL;
  new->active = 0;
  new->sourced = 0;
  for (i = 0; i < ELFSH_MAX_SOURCE_DEPTH; i++)
    {
      new->script[i] = NULL;
      new->lstcmd[i] = NULL;
    }

  new->createtime = time(&new->createtime);

  return new;
}


void		vm_switch_job(elfshjob_t      *job)
{
  world.curjob->active = 0;
  world.curjob = job;
  job->active = 1;

  return;
}


int		vm_valid_workspace(char *name)
{
  if (!name)
    return (0);
  
  if (!name[0])
    return (0);

  if (!strncmp(name, "net_init", 5))
    return (0);

  if (isdigit(name[0]))
    return (0);

  return (1);
}


int		vm_own_job(elfshjob_t *job)
{
  if (!job)
    return (0);

  if (job->io.type != world.curjob->io.type)
    return (0);

  if (job->io.input_fd != world.curjob->io.input_fd)
    return (0);

  if (job->io.output_fd != world.curjob->io.output_fd)
    return (0);
  
  return (1);
}


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
  
  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++);

  if (idx == 0)
    {
      /* $ workspace */
      vm_output(" .::. Workspaces .::. \n");
      for (index = 0; index < world.jobs.size; index++)
	{
	  for (actual = &world.jobs.ent[index];
	       actual != NULL && actual->key != NULL;
	       actual = actual->next)
	    {
	      if (vm_own_job((elfshjob_t *) actual->data))
		{
		  time = ctime(&((elfshjob_t *) actual->data)->createtime);
		  nl = strchr(time, '\n');
		  if (nl)
		    *nl = 0x00;

		  snprintf(logbuf, BUFSIZ - 1, " [%s] %s %c \n", actual->key, 
			   time,
			   (((elfshjob_t *) actual->data)->active?'*':' '));
		  vm_output(logbuf);
		  
		  if (((elfshjob_t *) actual->data)->list)
		    {
		      for (obj = ((elfshjob_t *) actual->data)->list; obj != NULL; obj = obj->next)
			{
			  snprintf(logbuf, BUFSIZ - 1, " \t %c %s \n", 
				   (((elfshjob_t *) actual->data)->current == obj)?'*':' ', 
				   obj->name);
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
	}
      vm_output("\n");
      return (0);
    }
  else if (idx == 1)
    {
      /* $ workspace name */
      if (vm_valid_workspace(world.curjob->curcmd->param[0]))
	{
	  if (!(job = (elfshjob_t *) hash_get(&world.jobs, world.curjob->curcmd->param[0])))
	    {
	      /* create a new workspace */
	      job = vm_clone_job(world.curjob);
	      hash_add(&world.jobs, world.curjob->curcmd->param[0], (void *)job);
	    }
	}

      job = hash_get(&world.jobs, world.curjob->curcmd->param[0]);
	  
      if (vm_own_job(job))
	{
	  /* switch */
	  snprintf(logbuf, BUFSIZ - 1, "\n [+] Workspace : %s \n\n", world.curjob->curcmd->param[0]);
	  vm_output(logbuf);
	  vm_switch_job(job);
	  return (0);
	}

	
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Incorrect workspace name", -1);
    }    
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong arg number", -1);
  return (0);
}
