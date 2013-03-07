/**
 * @defgroup io Librevm IO API.
 * @ingroup librevm
 */
/**
* @file librevm/io/io.c
 * @ingroup io
 * @brief Initialize the IO system.
 *
 * Started on  Fri Mar  5 00:55:40 2004 jfv
 * $Id$
 */
#include "revm.h"


/**
 * Reset lines counters and ignore output state 
 */
int		revm_flush()
{
  int  lines = 80;
  int  cols = 200;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Cache output only in IMODE/DEBUGGER mode */
  if (world.state.revm_mode != REVM_STATE_INTERACTIVE &&
      world.state.revm_mode != REVM_STATE_EMBEDDED)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  revm_screen_getsize(&lines, &cols);

  /* We * 2 because color take a lot of place !!! */
  world.curjob->ws.io.outcache.lines = lines * 2;
  world.curjob->ws.io.outcache.cols  = cols * 2;
  world.curjob->ws.io.outcache.nblines = lines;
  world.curjob->ws.io.outcache.ignore  = 0;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

/** 
 * Set workspace configuration to standard I/O 
 */
int		revm_std_io(revmjob_t *job)
{
  NOPROFILER_IN();
  if (!job)
    NOPROFILER_ROUT(0);
  job->ws.io.type      = REVM_IO_STD;
  job->ws.io.input_fd  = 0;
  job->ws.io.input     = revm_stdinput;
  job->ws.io.output_fd = 1;
  job->ws.io.output    = revm_stdoutput;
  NOPROFILER_ROUT(0);
}

/** 
 * Set workspace configuration to standard I/O 
 */
int		revm_fifo_io(revmjob_t *job)
{
  int		fd;
  int		fd2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!job)
    {
      fprintf(stderr, "error: input job is NULL in fifo_io \n");
      NOPROFILER_ROUT(0);
    }

  /* Remove the FIFO is already existing */
  if (world.state.revm_side == REVM_SIDE_CLIENT)
    {
      if (!access(REVM_FIFO_C2S, F_OK))
	unlink(REVM_FIFO_C2S);
      if (!access(REVM_FIFO_S2C, F_OK))
	unlink(REVM_FIFO_S2C);
      
      /* Create the 2 FIFO */
      mkfifo(REVM_FIFO_S2C, 0600);
      mkfifo(REVM_FIFO_C2S, 0600);
    }

  /* Register the FIFO as an I/O */
  if (!e2dbg_kpresence_get())
    {
      XOPEN(fd, REVM_FIFO_S2C, O_RDWR, 0600, -1);
      world.fifo_s2c = fd;
      XOPEN(fd2, REVM_FIFO_C2S, O_RDWR, 0600, -1);
      world.fifo_c2s = fd2;
      
      /* If we are in the embedded server part of the debugger, 
	 do all I/O on the FIFO */
      if (world.state.revm_side == REVM_SIDE_SERVER)
	{
	  job->ws.io.input_fd  = fd2;
	  job->ws.io.input     = revm_stdinput;
	  job->ws.io.output_fd = fd;
	  job->ws.io.output    = revm_stdoutput;
	  dup2(fd, 0);
	}
    }

  /* We dont let this between #ifdef's KERNEL to avoid recompiling
     this code multiple times for the userland and kernel debuggers */
  else
    {
      job->ws.io.input   = NULL;
      // FIXME: need IO vector ?
      //job->ws.io.output  = ke2dbg_output;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Initialize Input/Output hooks 
 */
int		revm_initio()
{
  static int	done = 0;
  revmjob_t	*initial;
  u_int		i;

  NOPROFILER_IN();
  if (done)
    NOPROFILER_ROUT(0);
  done = 1;

  XALLOC(__FILE__, __FUNCTION__, __LINE__,initial, sizeof(revmjob_t), -1);
  memset(initial, 0, sizeof(revmjob_t));

  hash_init(&initial->recur[0].exprs , "job0_rec0_exprs" , 23, ASPECT_TYPE_EXPR);
  hash_init(&initial->recur[0].labels, "job0_rec0_labels", 23, ASPECT_TYPE_STR);

  revm_std_io(initial);
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

  for (i = 0; i < REVM_MAXSRCNEST; i++)
    {
      initial->recur[i].script = NULL;
      initial->recur[i].lstcmd = NULL;
      initial->iter[i].elmidx = REVM_IDX_UNINIT;
    }

  initial->recur[0].funcname = "top-level";

  profiler_setcolor(revm_endline, revm_colorinstr, revm_colorstr, 
		    revm_colorfieldstr, revm_colortypestr, revm_colorend, 
		    revm_colorwarn, revm_colorfunction, revm_colorfilename);
  profiler_setmorecolor(revm_coloradv, revm_colorinstr_fmt, revm_coloraddress,
			revm_colornumber, revm_colorstr_fmt, 
			revm_colorfieldstr_fmt, 
			revm_colortypestr_fmt, revm_colorwarn_fmt);
  NOPROFILER_ROUT(0);
}
