/*
** input.c for librevm in ERESI
**
** Input related functions
**
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** $Id: input.c,v 1.6 2007-04-30 13:39:37 may Exp $
**
*/
#include "revm.h"



/* Read a new line, avoiding comments and void lines */
char		*vm_getln()
{
  char		*buf;
  char		*sav;
  
  NOPROFILER_IN();
  do
    {
      buf = world.curjob->ws.io.input();
      if (buf == ((char *) REVM_INPUT_VOID))
	NOPROFILER_ROUT((char *) REVM_INPUT_VOID);
      if (buf == NULL)
	NOPROFILER_ROUT(NULL);
      if (!*buf)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__,buf);
	  NOPROFILER_ROUT(NULL);
	}
      
      sav = buf;
      while (IS_BLANK(*sav))
	sav++;

      if (!*sav || *sav == REVM_COMMENT_START)
	{

	  vm_log(sav);
	  vm_log("\n");

#if !defined(USE_READLN)
	  XFREE(__FILE__, __FUNCTION__, __LINE__, buf);
#endif
	  
	  if (world.state.vm_mode == REVM_STATE_INTERACTIVE ||
	      world.state.vm_mode == REVM_STATE_DEBUGGER)
	    NOPROFILER_ROUT((char*) REVM_INPUT_VOID);

          buf = NULL;
          if (*sav)
	    continue;
	}

      if (world.state.vm_mode != REVM_STATE_SCRIPT)
	{
          vm_output_nolog("\n");

          /* avoid looping with readline */
#if defined(USE_READLN)
          if (buf == NULL)
	    {
	      //fprintf(stderr, "Entered readline test .. returning void input \n");
              NOPROFILER_ROUT((char *) REVM_INPUT_VOID);
	    }
          break;
#endif
	  
	}
    }
  while (buf == NULL);
  
  NOPROFILER_ROUT(buf);
}


/* Read input from the file descriptor */
char		*vm_read_input()
{
  char		tmpbuf[BUFSIZ + 1];
  int		len;

  NOPROFILER_IN();
  
  /* In case we are scripting, even readline will use a read */
  for (len = 0; len < BUFSIZ; len++)
    switch (read(world.curjob->ws.io.input_fd, tmpbuf + len, 1))
      {
      case 1:
	if (tmpbuf[len] == '\n')
	  {
	    if (len == 0)
	      {
		//fprintf(stderr, "Read length 0 ... \n");
		NOPROFILER_ROUT((char *) REVM_INPUT_VOID);
	      }
	    if (world.state.vm_mode == REVM_STATE_DEBUGGER &&
		world.state.vm_side == REVM_SIDE_CLIENT)
	      tmpbuf[len + 1] = 0x00;
	    else
	      tmpbuf[len] = 0x00;
	    goto end;
	  }
	continue;
      default:
	*tmpbuf = 0x00;
	goto end;
      }

 end:
  //fprintf(stderr, "[pid = %u] Read on fd = *%s*\n", getpid(), tmpbuf);
  NOPROFILER_ROUT((*tmpbuf ? strdup(tmpbuf) : NULL));
}


/* Input handler for the FIFO */
char		*vm_fifoinput()
{
  int		fd;
  char		*ret;

  fd = world.curjob->ws.io.input_fd;
  
  /* Just debugging */
  switch (world.state.vm_side)
    {
    case REVM_SIDE_CLIENT:
      //fprintf(stderr, "Goto read fifo (client legit) ... \n");      
      world.curjob->ws.io.input_fd = world.fifo_s2c;
      break;
    case REVM_SIDE_SERVER:
      //fprintf(stderr, "Goto read fifo (server legit) ... \n");
      world.curjob->ws.io.input_fd = world.fifo_c2s;
      break;
    }
 
 ret = vm_read_input();
 world.curjob->ws.io.input_fd = fd;
 world.curjob->ws.io.input = world.curjob->ws.io.old_input;
 
 /* Just debugging */ 
  switch (world.state.vm_side)
    {
    case REVM_SIDE_CLIENT:
      //fprintf(stderr, "BACK from reading fifo (client legit) ... \n");      
      break;
    case REVM_SIDE_SERVER:
      //fprintf(stderr, "BACK from reading fifo (server legit) ... \n");
      break;
    }

  return (ret);
}


/* INPUT handler for stdin */
char		*vm_stdinput()
{
  char		*str;

  NOPROFILER_IN();
  str = NULL;

  /* Case if we are using readline */
#if defined(USE_READLN)
  if (world.state.vm_mode != REVM_STATE_SCRIPT)
    {
      str = readln_input_check();
      NOPROFILER_ROUT(str);
    }
#endif

  /* If not, read the stdin file descriptor */
  NOPROFILER_ROUT(vm_read_input());
}


/* Change the Input file */
void	vm_setinput(revmworkspace_t *ws, int fd)
{
  ws->io.input_fd = fd;
}
