/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
**
** $Id: job.c,v 1.3 2007-07-17 18:11:24 may Exp $
**
*/
#include "revm.h"


/* Create a new job structure */
revmjob_t	*revm_clone_job(char *newname, revmjob_t *job)
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
  new->sourced        = 0;
  new->ws.active      = 0;
  new->ws.oldline     = NULL;
  new->ws.screen.buf  = new->ws.screen.tail = new->ws.screen.head = NULL;

  for (i = 0; i < REVM_MAXSRCNEST; i++)
    {
      new->script[i] = NULL;
      new->lstcmd[i] = NULL;
    }
  new->ws.createtime = time(&new->ws.createtime);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/* Switch of current job */
void		revm_switch_job(revmjob_t      *job)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Save the active buffer line */
#if defined(USE_READLN)
  if (world.curjob->ws.io.savebuf)
    XFREE(__FILE__, __FUNCTION__, __LINE__,world.curjob->ws.io.savebuf);
  world.curjob->ws.io.savebuf = strdup(rl_line_buffer); 
  world.curjob->ws.io.buf = NULL;
  world.curjob->ws.io.rl_point = rl_point;
  world.curjob->ws.io.rl_end = rl_end;
#endif

  world.curjob->ws.active = 0;
  world.curjob = job;
  job->ws.active = 1;

#if defined(USE_READLN)
  rl_set_prompt(revm_get_prompt());
#endif

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Is this workspace valid for switching ? */
int		revm_valid_workspace(char *name)
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
int		revm_own_job(revmjob_t *job)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!job)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->ws.io.type != world.curjob->ws.io.type)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->ws.io.input_fd != world.curjob->ws.io.input_fd)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (job->ws.io.output_fd != world.curjob->ws.io.output_fd)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}
