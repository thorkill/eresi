/*
** readln.c elfsh
** 
** Started on  Tue Feb 18 06:24:42 2003 emsi
** Updated on  Mon Sep 19 06:57:03 2005 maym
*/
#include "elfsh.h"


#if defined(USE_READLN)


/* Completion fonction for the command list */
char		*command_generator(const char *text, int state)
{
  static int	i, len, tab;
  char		*name;
  const char    *baq;

  baq = text;
  for (name = strchr(baq, '.'); name; name = strchr(baq, '.'))
    {
      baq = name;
      baq++;
    }
  text = baq;

  //printf("\n trying to match %s\n", text);

  if (!state)
    {
      tab = 0;
      i   = 0;
      len = strlen(text);
    }
  else
    i++;

  /* Return the next name which partially matches any hash tables keys */
  for (; tab < ELFSH_COMPMAX; i = 0, tab++)
    if (world.comp.cmds[tab] != NULL)
      for (; world.comp.cmds[tab][i] != NULL; i++)
	{
	  name = world.comp.cmds[tab][i];
	  if (!strncmp(name, text, len))
	    return (strdup(name));
	}
  
  /* If no names matched, then return NULL. */
  i = tab = len = 0;
  return ((char *) NULL);
}



char	**custom_completion(const char* text, int start, int end) 
{
  char	**matches = (char**) NULL;
  char	*baq, *baq2;
  char	buf[50];
  int	len;

#if defined(__OpenBSD__)
  matches = completion_matches(text, command_generator);
#else
  matches = rl_completion_matches(text, command_generator);
#endif

  if (!matches)
    return (NULL);

  baq2 = NULL;
  baq = strchr(text, '.');
  while (baq)
    {
      baq2 = baq + 1;
      baq = strchr(baq2, '.');
    }
  if (baq2 && *baq2)
    {
      memcpy(buf, text, baq2 - text);
      strcpy(buf + (baq2 - text), matches[0]);
      free(matches[0]);
      matches[0] = strdup(buf);
    }
  
  switch (rl_completion_append_character)
    {
    case ' ':
      if (baq2 && *baq2)
	rl_completion_append_character = '[';
      break;
    case '[':
      rl_completion_append_character = ']';
      break;
    case ']':
      rl_completion_append_character = ' ';
    default:
      break;
    }

  return (matches);
}




#endif

