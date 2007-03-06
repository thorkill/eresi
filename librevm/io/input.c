/*
** input.c for librevm in ERESI
**
** Input related functions
**
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** Updated on  Fri Mar  5 18:47:41 2007 mayhem
*/
#include "revm.h"



/* Read a new line, avoiding comments and void lines */
char		*vm_getln(char *ptr)
{
  char		*buf;
  char		*sav;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  do
    {
      buf = world.curjob->ws.io.input();

      if (buf == ((char *) REVM_INPUT_VOID))
        PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (char*) REVM_INPUT_VOID);
      
      if (!buf || !*buf)
	{
	  if (buf)
	    XFREE(__FILE__, __FUNCTION__, __LINE__,buf);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
	}

      if (buf == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot read input file", NULL);

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
               PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
                (char*) REVM_INPUT_VOID);


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
              PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
	       (char*) REVM_INPUT_VOID);
	    }
          break;
#endif

	}
    }
  while (buf == NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,buf);
}




/* INPUT handler for stdin */
char		*vm_stdinput()
{
  char		tmpbuf[BUFSIZ + 1];
  int		len;
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  str = NULL;

#if defined(USE_READLN)
  if (world.state.vm_mode != REVM_STATE_SCRIPT)
    {
      str = readln_input_check();
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, str);
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
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
			    ((char *) REVM_INPUT_VOID));
	    tmpbuf[len] = 0x00;
	    goto end;
	  }
	continue;
      default:
	*tmpbuf = 0x00;
	goto end;
      }

 end:
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		(*tmpbuf ? strdup(tmpbuf) : NULL));
}


/* Change the Input file */
void	vm_setinput(revmworkspace_t *ws, int fd)
{
  ws->io.input_fd = fd;
}



