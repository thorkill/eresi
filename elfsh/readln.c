/*
** readlinestuff.c elfsh
** 
** Started on  Tue Feb 18 06:24:42 2003 emsi
*/
#include "elfsh.h"



/* Completion fonction for the command list */
char *command_generator(const char* text, int state)
{
  static int i, len;
  char *name;
  
  if (!state)
    {
      i = 0;
      len = strlen (text);
    }
  
  /* Return the next name which partially matches from the
     command list. */
  while (world.cmds[i]!=NULL)
    {
      name=world.cmds[i];
      i++;
      
      if (strncmp (name, text, len) == 0)
	return (strdup(name));
    }
  
  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}



#if defined(USE_READLN)
char	**custom_completion(const char* text, int start, int end) 
{
  char	**matches = (char**) NULL;
  
  if (start == 0)
#if defined(__OpenBSD__)
    matches = completion_matches (text, command_generator);
#else
  matches = rl_completion_matches (text, command_generator);
#endif
  return (matches);
}
#endif


