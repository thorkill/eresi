/*
** io.c for librevm in ERESI
**
** Initialize the IO system
**
** Started on  Fri Mar  5 00:55:40 2004 mayhem
** Updated on  Mon Mar  5 18:47:41 2005 ym
*/
#include "revm.h"


/* Reset lines counters and ignore output state */
int		vm_flush()
{
  int  lines = 80;
  int  cols = 200;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Cache output only in IMODE/DEBUGGER mode */
  if (world.state.vm_mode != REVM_STATE_INTERACTIVE &&
      world.state.vm_mode != REVM_STATE_DEBUGGER)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

#ifdef USE_READLN
  rl_get_screen_size(&lines, &cols);
#endif

  /* We * 2 because color take a lot of place !!! */
  world.curjob->ws.io.outcache.lines = lines * 2;
  world.curjob->ws.io.outcache.cols  = cols * 2;
  world.curjob->ws.io.outcache.nblines = lines;
  world.curjob->ws.io.outcache.ignore  = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

/* Set workspace configuration to standard I/O */
int		vm_std_io(revmjob_t *job)
{
  NOPROFILER_IN();
  if (!job)
    NOPROFILER_ROUT(0);
  job->ws.io.type      = ELFSH_IOSTD;
  job->ws.io.input_fd  = 0;
  job->ws.io.input     = vm_stdinput;
  job->ws.io.output_fd = 1;
  job->ws.io.output    = vm_stdoutput;
  NOPROFILER_ROUT(0);
}

/* Set workspace configuration to standard I/O */
int		vm_fifo_io(revmjob_t *job)
{
  int		fd;

  NOPROFILER_IN();
  if (!job)
    {
      fprintf(stderr, "error: input job is NULL in fifo_io \n");
      NOPROFILER_ROUT(0);
    }

  mkfifo(REVM_FIFO, 0600);
  XOPEN(fd, REVM_FIFO, 0600, 0, -1);

  job->ws.io.type      = ELFSH_IOFIFO;
  job->ws.io.input_fd  = fd;
  job->ws.io.input     = vm_stdinput;
  job->ws.io.output_fd = fd;
  job->ws.io.output    = vm_stdoutput;
  NOPROFILER_ROUT(0);
}


/* Initialize Input/Output hooks */
int		vm_initio()
{
  static int	done = 0;
  revmjob_t	*initial;

  NOPROFILER_IN();
  if (done)
    NOPROFILER_ROUT(0);
  done = 1;

  XALLOC(__FILE__, __FUNCTION__, __LINE__,initial, sizeof(revmjob_t), -1);
  memset(initial, 0, sizeof(revmjob_t));

  vm_std_io(initial);
  initial->ws.active	   = 1;
  initial->ws.createtime   = time(&initial->ws.createtime);

  world.initial = world.curjob = initial;
  hash_init(&world.jobs, "jobs", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&world.jobs, "local", initial);
  initial->ws.name = strdup("local");
 
  hash_init(&initial->loaded,    
	    "initial_loaded_files", 
	    51, ASPECT_TYPE_UNKNOW);
  hash_init(&initial->dbgloaded,
	    "initial_dbgloaded_files",
	    11, ASPECT_TYPE_UNKNOW);

  profiler_setcolor(vm_endline, vm_colorinstr, vm_colorstr, 
		    vm_colorfieldstr, vm_colortypestr, vm_colorend, 
		    vm_colorwarn, vm_colorfunction, vm_colorfilename);
  profiler_setmorecolor(vm_coloradv, vm_colorinstr_fmt, vm_coloraddress,
			vm_colornumber, vm_colorstr_fmt, 
			vm_colorfieldstr_fmt, 
			vm_colortypestr_fmt, vm_colorwarn_fmt);
  NOPROFILER_ROUT(0);
}
