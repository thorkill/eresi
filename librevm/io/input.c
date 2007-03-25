/*
** input.c for librevm in ERESI
**
** Input related functions
**
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** Updated on  Fri Mar  5 18:47:41 2007 mayhem
**
** $Id: input.c,v 1.5 2007-03-25 14:27:34 may Exp $
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
	      printf("Entered readline test .. returning void input \n");
              NOPROFILER_ROUT((char *) REVM_INPUT_VOID);
	    }
          break;
#endif
	  
	}
    }
  while (buf == NULL);
  
  NOPROFILER_ROUT(buf);
}




/* INPUT handler for stdin */
char		*vm_stdinput()
{
  char		tmpbuf[BUFSIZ + 1];
  int		len;
  char		*str;

  NOPROFILER_IN();
  str = NULL;

#if defined(USE_READLN)
  if (world.state.vm_mode != REVM_STATE_SCRIPT)
    {
      str = readln_input_check();
      NOPROFILER_ROUT(str);
    }
#endif
  
  /* In case we are scripting, even readline will use a read */
  for (len = 0; len < BUFSIZ; len++)
    switch (read(world.curjob->ws.io.input_fd, tmpbuf + len, 1))
      {
      case 1:
	if (tmpbuf[len] == '\n')
	  {
	    if (len == 0)
	      NOPROFILER_ROUT((char *) REVM_INPUT_VOID);
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
  NOPROFILER_ROUT((*tmpbuf ? strdup(tmpbuf) : NULL));
}


/* Change the Input file */
void	vm_setinput(revmworkspace_t *ws, int fd)
{
  ws->io.input_fd = fd;
}



