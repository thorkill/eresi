/*
** lexer.c for elfsh
** 
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** Updated on  Fri Mar  5 18:47:41 2007 mayhem
*/
#include "revm.h"


/* Replace \xNUM by the value, I wished readline could have done that */
void			vm_findhex(u_int argc, char **argv)
{
  u_int			index;
  char			*buf;
  char			*ptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* For each of the argv[] entry */
  for (index = 0; index < argc; index++)
    {
      if (argv[index] == NULL)
	continue;
      buf = argv[index];

      /* Find "\x" sequences */
      for (ptr = strstr(buf, "\\x"); ptr != NULL; ptr = strstr(buf, "\\x"))
	buf = vm_filter_param(buf, ptr);
    }
   PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Count blanks, so that we can allocate argv */
u_int		vm_findblanks(char *buf)
{
  int		index;
  int		len;
  int		nbr;
  char		*ptr;
  char		*sav;
#if __DEBUG_SCANNER__
  char		logbuf[BUFSIZ];
#endif

  len = strlen(buf);
  index = 0;
  nbr = 1;
  sav = buf;
  do
    {
      while (IS_BLANK(*sav))
	sav++;
      if (!*sav)
	break;
      ptr = strchr(sav, ' ');
      if (!ptr)
	{
	  ptr = strchr(sav, '\t');
	  if (!ptr)
	    break;
	}
      while (IS_BLANK(*ptr))
	ptr++;
      if (!*ptr)
	break;
      nbr++;
      sav = ptr;
    }
  while (ptr && sav < buf + len);

#if __DEBUG_SCANNER__
  snprintf(logbuf, BUFSIZ - 1, "[DEBUG_SCANNER:findblanks] nbr = %u \n", nbr);
  vm_output(logbuf);
#endif

  return (nbr);
}


/* Cut words of the newline and create argv */
char		**vm_doargv(u_int nbr, u_int *argc, char *buf)
{
  u_int		index;
  char		*sav;
  char		**argv;
  char		*ptr;
#if __DEBUG_SCANNER__
  char		logbuf[BUFSIZ];
#endif

  NOPROFILER_IN();

  XALLOC(__FILE__, __FUNCTION__, __LINE__,argv, sizeof(char *) * (nbr + 2), NULL);
  argv[0] = argv[nbr + 1] = NULL;
  sav = buf;

  for (index = 1; index < nbr + 1; index++)
    {

      assert(sav >= buf);

      while (IS_BLANK(*sav))
	sav++;
      argv[index] = sav;

#if __DEBUG_SCANNER__
      snprintf(logbuf, BUFSIZ - 1,
	       "[DEBUG_SCANNER:lexer_doargv] Adding argv[%u] = *%s* \n", 
	       index, sav);
      vm_output(logbuf);
#endif 

      ptr = strchr(sav, ' ');
      if (!ptr)
	ptr = strchr(sav, '\t');
      if (ptr)
	{
	  *ptr = 0;
	  sav = ptr + 1;
	}
    }

#if __DEBUG_SCANNER__
  vm_output("\n");
  for (index = 0; index < nbr + 1; index++)
    if (argv[index])
      {
	snprintf(logbuf, BUFSIZ - 1, "[DEBUG_SCANNER:lexer_doargv] %u/ *%s* \n",
		 index, argv[index]);
	vm_output(logbuf);
      }
  vm_output("\n");
#endif

  *argc = nbr + 1;

  NOPROFILER_ROUT(argv);
}








/* Its lighter than flex ... */
char		**vm_input(int *argc)
{
  char		**argv;
  char		*buf;
  int		nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Read, and sanitize a first time to avoid blank lines */
  buf = ((world.state.vm_mode == REVM_STATE_INTERACTIVE ||
	  world.state.vm_mode == REVM_STATE_DEBUGGER) ?
	 vm_get_prompt() : "");
  buf = vm_getln(buf);
  if (!buf)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(NULL));

  /* Blank Line : return the last command */
  if (buf == (char *) REVM_INPUT_VOID)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
	 ((char **) REVM_INPUT_VOID));

  if (world.state.vm_mode != REVM_STATE_SCRIPT &&
      world.curjob->ws.io.type == ELFSH_IOSTD)
    {
#if defined(USE_READLN)
      readln_input_log(buf);
#else
      vm_log(buf);
      vm_log("\n\n");
#endif
    }

  /* Save the line for future references */
  if (world.curjob->ws.oldline && *world.curjob->ws.oldline)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, 
	    world.curjob->ws.oldline);
      world.curjob->ws.oldline = NULL;
    }
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 world.curjob->ws.oldline, strlen(buf) + 1, NULL);
  strcpy(world.curjob->ws.oldline, buf);

  /* Allocate the correct pointer array for argv */
  nbr = vm_findblanks(buf);
  argv = vm_doargv(nbr, (u_int *) argc, buf);

  /* Find and replace "\xXX" sequences, then return the array */
  vm_findhex(*argc, argv);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (argv));
}
