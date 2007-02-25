/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
*/
#include "libui.h"


/* Create a new job structure */
revmjob_t	*vm_clone_job(char *newname, revmjob_t *job)
{
  revmjob_t    *new;
  int		i;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmjob_t), NULL);
  memcpy(new, job, sizeof(revmjob_t));
  bzero(&new->loaded, sizeof(hash_t));
  bzero(&new->dbgloaded, sizeof(hash_t));

  snprintf(logbuf, sizeof(logbuf), "%s_loaded", newname);
  hash_init(&new->loaded, strdup(logbuf), 51, ASPECT_TYPE_UNKNOW);
 
  snprintf(logbuf, sizeof(logbuf), "%s_dbgloaded", newname);
  hash_init(&new->dbgloaded, strdup(logbuf), 11, ASPECT_TYPE_UNKNOW);
 

  /* empty new job */
  new->curcmd         = NULL;
  new->active         = 0;
  new->sourced        = 0;
  new->oldline        = NULL;
  new->screen.buf     = new->screen.tail = new->screen.head = NULL;

  for (i = 0; i < ELFSH_MAX_SOURCE_DEPTH; i++)
    {
      new->script[i] = NULL;
      new->lstcmd[i] = NULL;
    }
  new->createtime = time(&new->createtime);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/* Switch of current job */
void		vm_switch_job(revmjob_t      *job)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  /* Save the active buffer line */
  if (world.curjob->io.savebuf)
    XFREE(__FILE__, __FUNCTION__, __LINE__,world.curjob->io.savebuf);
  world.curjob->io.savebuf = strdup(rl_line_buffer);
 
  world.curjob->io.buf = NULL;
  world.curjob->io.rl_point = rl_point;
  world.curjob->io.rl_end = rl_end;
#endif

  world.curjob->active = 0;
  world.curjob = job;
  job->active = 1;

#if defined(USE_READLN)
  rl_set_prompt(vm_get_prompt());
#endif

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Is this workspace valid for switching ? */
int		vm_valid_workspace(char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!name)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!name[0])
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!strncmp(name, "net_init", 5))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (isdigit(name[0]))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/* Is this workspace the current one ? */
int		vm_own_job(revmjob_t *job)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!job)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.type != world.curjob->io.type)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.input_fd != world.curjob->io.input_fd)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->io.output_fd != world.curjob->io.output_fd)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


