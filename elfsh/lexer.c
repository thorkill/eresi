/*
** lexer.c for elfsh
** 
** Started on  Fri Feb  7 20:53:25 2003 mayhem
** Last update Wed Jun 25 08:47:37 2003 mayhem
*/
#include "elfsh.h"


/* Replace \xNUM by the value, I wished readline could have done that */
static void		lexer_findhex(u_int argc, char **argv)
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
static char	*lexer_getln(char *ptr)
{
  char		*buf = NULL;
  char		*sav;
  char		tmpbuf[BUFSIZ];
  
  do
    {

#if defined(USE_READLN)
      if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
	buf = readline(ptr);
      else
	{
#endif
	  printf("%s", ptr);
	  fflush(stdout);
	  if (fgets(tmpbuf, sizeof(tmpbuf) - 1, stdin) == NULL)
	    {
	      fprintf(stderr, "[!] Fatal parser error [cant read inputfile]\n\n");
	      exit(-1);
	    } 
	  sav = strchr(tmpbuf, '\n');
	  if (sav != NULL)
	    *sav = 0x00;
	  if (!*tmpbuf)
	    continue;
	  buf = strdup(tmpbuf);

#if defined(USE_READLN)
	}
#endif
      
      if (buf == NULL)
	{
	  fprintf(stderr, "[!] Fatal readline error [cant read inputfile]\n\n");
	  exit(-1);
	}

      sav = buf;
      while (IS_BLANK(*sav))
	sav++;
      if (!*sav || *sav == ELFSH_COMMENT_START)
	{

#if defined(USE_READLN)
	  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)  
#endif
	    free(buf);
	  
	  buf = NULL;
	  if (*sav)
	    continue;	
	}
      putchar('\n');
    }
  while (buf == NULL);
  return (buf);
}


/* Count blanks, so that we can allocate argv */
static u_int	lexer_findblanks(char *buf)
{
  int		index;
  int		len;
  int		nbr;
  char		*ptr;
  char		*sav;

  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    {
      printf("~%s\n", buf);
      fflush(stdout);
    }
  len = strlen(buf);
  index = 0;
  nbr = 1;
  sav = buf;
  do
    {
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
      sav = ptr + 1;
    }
  while (ptr && sav < buf + len);

#if __DEBUG_SCANNER__
  printf("[DEBUG_SCANNER:findblanks] nbr = %u \n", nbr);
#endif

  return (nbr);
}


/* Cut words of the newline and create argv */
static char	**lexer_doargv(u_int nbr, u_int *argc, char *buf)
{
  u_int		index;
  char		*sav;
  char		**argv;
  char		*ptr;

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
      printf("[DEBUG_SCANNER:lexer_doargv] Adding argv[%u] = *%s* \n", 
	     index, sav);
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
  puts("");
  for (index = 0; index < nbr + 1; index++)
    printf("[DEBUG_SCANNER:lexer_doargv] %u/ *%s* \n", index, argv[index]);
  puts("");
#endif

  *argc = nbr + 1;
  return (argv);
}



/* I was too lazy to use flex for the moment, pardon me ;) */
char		**vm_getln(int *argc)
{
  char		**argv;
  char		*buf;
  int		nbr;

  /* Read, and sanitize a first time to avoid blank lines */
  buf = (world.state.vm_mode == ELFSH_VMSTATE_IMODE ? ELFSH_PROMPT : "");
  buf = lexer_getln(buf);
#if defined(USE_READLN)
  add_history (buf);
#endif

  /* Allocate the correct pointer array for argv */
  nbr = lexer_findblanks(buf);
  argv = lexer_doargv(nbr, argc, buf);

  /* Find and replace "\xXX" sequences, then return the array */
  lexer_findhex(*argc, argv);
  return (argv);
}








