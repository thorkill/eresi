/*
** lexer.c for elfsh
** 
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** Updated on  Fri Mar 11 18:47:41 2005 ym
*/

#include "libui.h"


/* Replace \xNUM by the value, I wished readline could have done that */
void			vm_findhex(u_int argc, char **argv)
{
  u_int			index;
  char			*buf;
  char			*ptr;

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
}


/* Read a new line, avoiding comments and void lines */
char		*vm_getln(char *ptr)
{
  char		*buf;
  char		*sav;
  char		logbuf[BUFSIZ];

  do
    {
      buf = world.curjob->io.input();
      
      if (buf == ((char *) ELFSH_VOID_INPUT))
        return ((char *) ELFSH_VOID_INPUT);

      if (!buf || !*buf)
	{
	  if (buf)
	    XFREE(buf);
	  return (NULL);
	}

      if (buf == NULL)
	{
          snprintf(logbuf, BUFSIZ - 1,
                   "[!] Fatal readline error [cant read inputfile]\n\n");
          vm_output(logbuf);
          vm_exit(-1);
	}
      
      sav = buf;
      while (IS_BLANK(*sav))
	sav++;

      if (!*sav || *sav == ELFSH_COMMENT_START)
	{
	  
	  vm_log(sav);
	  vm_log("\n");

#if defined(USE_READLN)
          if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)  
#endif
	    XFREE(buf);

	  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE ||
	      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
	    return ((char *) ELFSH_VOID_INPUT); 

          buf = NULL;

          if (*sav)
	    continue;	
	}
      
      if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
	{
          vm_output_nolog("\n"); 

          /* avoid looping with readline */
#if defined(USE_READLN)
          if (buf == NULL)
	    {
	      printf("Entered readline test .. returning void input \n");
	      return ((char *) ELFSH_VOID_INPUT); 
	    }
          break;
#endif

	}
    }
  while (buf == NULL);
  return (buf);
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

  ELFSH_NOPROFILE_IN();

  XALLOC(argv, sizeof(char *) * (nbr + 2), NULL);
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

  ELFSH_NOPROFILE_ROUT(argv);
}





