/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
*/
#include "libui.h"




elfshjob_t	*vm_clone_job(elfshjob_t      *job)
{
  elfshjob_t    *new;
  int		i;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(new, sizeof(elfshjob_t), NULL);

  memcpy(new, job, sizeof(elfshjob_t));
  
  /* empt?? new job */
  new->list = NULL;
  new->current = NULL;
  new->curcmd = NULL;
  new->active = 0;
  new->sourced = 0;

#if defined(USE_READLN)
  new->screen.buf = new->screen.tail = new->screen.head = NULL;
#endif

  for (i = 0; i < ELFSH_MAX_SOURCE_DEPTH; i++)
    {
      new->script[i] = NULL;
      new->lstcmd[i] = NULL;
    }

  new->createtime = time(&new->createtime);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


void		vm_switch_job(elfshjob_t      *job)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
#if defined(USE_READLN)
  world.curjob->io.buf = NULL;
#endif
  world.curjob->active = 0;
  world.curjob = job;
  job->active = 1;
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


int		vm_valid_workspace(char *name)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!name)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  if (!name[0])
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!strncmp(name, "net_init", 5))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (isdigit(name[0]))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


int		vm_own_job(elfshjob_t *job)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!job)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.type != world.curjob->io.type)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.input_fd != world.curjob->io.input_fd)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.output_fd != world.curjob->io.output_fd)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}
