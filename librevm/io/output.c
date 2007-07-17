/*
** output.c for librevm in ERESI
**
** The output vector interface
**
** Started on Fri Mar 5 00:55:40 2004 mayhem
** Updated on Mon Mar 5 18:47:41 2007 mayhem
**
** $Id: output.c,v 1.5 2007-07-17 18:11:25 may Exp $
**
*/
#include "revm.h"



/* Display str on all term */
int		revm_output_bcast(char *str)
{
  int		index;
  int		ret = 0;
  revmjob_t	*old;
  char		**keys;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Saving current output parameters */
  old = world.curjob;

  /* Net's outputs */
  if (world.state.revm_net)
    {
      keys = hash_get_keys(&world.jobs, &keynbr);
      for (index = 0; index < keynbr; index++)
        {
	  old = hash_get(&world.jobs, keys[index]);
	  if (!strcmp(keys[index], "local") || !strcmp(keys[index], "net_init") ||
	      !strncmp(keys[index], "DUMP", 4) || !old->ws.active);
	  continue;
	  world.curjob = old;
	  ret |= revm_output(str);
	  revm_flush();
	}
    }

  /* stdout */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    {
      world.curjob = hash_get(&world.jobs, "local");
      ret |= revm_output(str);
    }

  /* Restore */
  world.curjob = old;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/* OUTPUT handler for stdout */
int		revm_output(char *str)
{
  char		*tmp;
  char		c;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_log(str);

  /* No -- more -- in some modes */
  if ((world.state.revm_mode != REVM_STATE_INTERACTIVE &&
       world.state.revm_mode != REVM_STATE_DEBUGGER)
      || world.curjob->ws.io.type == REVM_IO_DUMP
      || !world.curjob->ws.io.outcache.lines
      || world.curjob->sourced
      || !(int)config_get_data(ELFSH_VMCONFIG_USE_MORE))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
	 (world.curjob->ws.io.output(str)));

  /* Discard outputs */
  if (world.curjob->ws.io.outcache.ignore)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  /* Counts lines */
  tmp = strchr(str, '\n');
  while (tmp)
    {
      world.curjob->ws.io.outcache.nblines--;
      if (*tmp == '\0')
	break;
      tmp ++;
      tmp = strchr(tmp, '\n');
    }

  /* Do the output */
  ret = world.curjob->ws.io.output(str);

  /* Is there any lines remaining ? */
  if (world.curjob->ws.io.outcache.nblines < 0)
    {
      revm_flush();
      tmp = "-- press enter for more ('q/n' to quit / next) --\n";
      world.curjob->ws.io.output(tmp);

      /* We decided to discard further output (until next revm_flush) */
      if ((read(world.curjob->ws.io.input_fd, &c, 1) == 1) && (c == 'q' || c == 'n'))
	{
	  if (c == 'q')
	    world.curjob->ws.io.outcache.ignore = 1;
	  world.curjob->ws.io.output("\n");
	  revm_log("\n");
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  
			     (c == 'q' ? -1 : -2));
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  ret);
}



/* Output without buffering/log */
int		revm_output_nolog(char *str)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     world.curjob->ws.io.output(str));
}



/* ERR output function (stderr) */
int		revm_outerr(char *str)
{
  revm_log(str);
  fprintf(stderr, "%s", str);
  return (0);
}



/* OUTPUT handler for stdout */
int		revm_stdoutput(char *str)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  printf("%s", str);
  fflush(stdout);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,0);
}


/* Change the Output file */
void	revm_setoutput(revmworkspace_t *ws, int fd)
{
  ws->io.output_fd = fd;
}

