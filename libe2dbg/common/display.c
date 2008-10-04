/*
** display.c for e2dbg
**    
** Started on  Tue Aug 16 09:38:03 2005 jfv                                                                                                                   
**
** $Id: display.c,v 1.1 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libe2dbg.h"


/* Print all display commands */
void		e2dbg_display_print()
{
  char		buff[BUFSIZ];
  int		idx;
  int		printed;

  listent_t     *actual;
  elfshbp_t     *curbp;
  int		idx2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Global displays */
  printed = 0;
  for (idx = 0; idx < E2DBG_STEPCMD_MAX; idx++)
    {
      if (!e2dbgworld.displaycmd[idx])
	continue;
      if (!printed)
	{
	  e2dbg_output("\n\t .:: e2dbg global displayed commands \n");
	  printed = 1;
	}
      snprintf(buff, BUFSIZ, "\t %u: %s \n", 
	       idx, e2dbgworld.displaycmd[idx]);
      e2dbg_output(buff);
    }
  if (!printed)
    e2dbg_output("\n\t .:: e2dbg currently has no global displays \n\n");
  
  /* Display breakpoint commands */
  printed = 0;
  for (idx = 0; idx < e2dbgworld.bp.size; idx++)
    for (actual = e2dbgworld.bp.ent + idx;
	 actual != NULL && actual->key != NULL;
	 actual = actual->next)
      {
	curbp = (elfshbp_t *) actual->data;
	for (idx2 = 0; idx2 < 10; idx2++)
	  if (curbp->cmd[idx2])
	    {
	      if (!printed)
		{
		  snprintf(buff, BUFSIZ, 
			   "\n\t .:: e2dbg display for breakpoint %u [" XFMT "] %s \n",
			   curbp->id, curbp->addr, curbp->symname);
		  e2dbg_output(buff);
		  printed = 1;
		}
	      snprintf(buff, BUFSIZ, "\t %u: %s \n",
		       idx2, curbp->cmd[idx2]);
	      e2dbg_output(buff);
	    }
	printed = 0;
      }

  e2dbg_output("\n");
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/* Execute all displayed commands for a breakpoint or a step */
int		e2dbg_display(char **cmd, unsigned int nbr)
{
  int		idx;
  revmargv_t	*cur;
  char		*str;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (idx = 0; idx < nbr; idx++)
    {
      fprintf(stderr, "executing display entry %u \n", idx);      
      if (cmd[idx])
	{
	  
	  snprintf(buf, BUFSIZ, 
		   "\t .:: Display %u [%s] result ::. \n", 
		   idx, cmd[idx]);
	  e2dbg_output(buf);
	  
	  /* Register displayed command in the script control flow */
	  str = strdup(cmd[idx]);
	  
	  if (revm_exec_str(cmd[idx]) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Display execrequest failed", -1);
	  cmd[idx] = str;
	  
	  /* Execute displayed command */
	  cur = world.curjob->curcmd;
	  world.curjob->curcmd = world.curjob->script[world.curjob->sourced]; 
	  if (revm_execmd() < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Display execution failed", -1);
	  world.curjob->curcmd = cur;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Display on breakpoint or step */
int		cmd_display()
{
  elfshbp_t	*bp;
  char		buf[BUFSIZ];
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* No parameter, list displays */
  if (!world.curjob->curcmd->param[0])
    {
      e2dbg_display_print();
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  }
  
  /* Add a global display */
  if (!revm_isnbr(world.curjob->curcmd->param[0]))
    {
      if (e2dbgworld.displaynbr >= E2DBG_STEPCMD_MAX)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Too many global displays", 0);
      str = revm_string_get(world.curjob->curcmd->param);
      snprintf(buf, BUFSIZ, "\n [*] Adding global display %u [%s] \n\n", 
	       e2dbgworld.displaynbr, str);
      e2dbg_output(buf);
      e2dbgworld.displaycmd[e2dbgworld.displaynbr++] = str;
    }
  
  /* Add a local display on breakpoint */
  else
    {
      bp = e2dbg_breakpoint_lookup(world.curjob->curcmd->param[0]);
      if (!bp)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find specified breakpoint", 0);
      if (bp->cmdnbr >= 10)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Too many local displays", 0);
      str = revm_string_get(&world.curjob->curcmd->param[1]);
      snprintf(buf, BUFSIZ, "\n [*] Adding display %u [%s] for breakpoint %u "
	       "[" XFMT "] %s \n\n", bp->cmdnbr, str, 
	       bp->id, bp->addr, bp->symname);
      e2dbg_output(buf);
      bp->cmd[(int) bp->cmdnbr++] = str;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Delete displays */
int		cmd_undisplay()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);



  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
