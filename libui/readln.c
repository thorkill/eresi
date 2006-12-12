/*
** readln.c elfsh
**
** Started on  Tue Feb 18 06:24:42 2003 emsi
** Updated on  Fri Feb 18 23:59:25 2006 thorkill
** Updated on  Tue Jun 27 23:51:04 2006 mxatone
*/
#include "libui.h"

#if defined(USE_READLN)


/* Completion fonction for the command list */
char		*command_generator(const char *text, int state)
{
  static int	i, len, tab;
  char		*name, *name2;
  const char    *baq;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
	    {
	      name2 = vm_readline_malloc(strlen(name) + 1);
	      strcpy(name2, name);
	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, name2);
	    }
	}

  /* If no names matched, then return NULL. */
  i = tab = len = 0;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,  ((char *) NULL));
}



char	**custom_completion(const char* text, int start, int end) 
{
  char	**matches = (char**) NULL;
  char	*baq, *baq2;
  char	buf[50];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* prevent freeing of unitialized memory on FreeBSD
	 XXX: check this !!
   */

  if (strlen(text) == 0)
   ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

#if defined(__OpenBSD__)
  matches = completion_matches(text, command_generator);
#else
  matches = rl_completion_matches(text, command_generator);
#endif

  if (!matches)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

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
      XFREE(matches[0]);
      matches[0] = elfsh_strdup(buf);
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, matches);
}


/* This function was used to update columns on a readline colored prompt
   another solution has been found that work well (see vm_rl_update_prompt).
   We keep this function because it can be useful later. */
int 	update_col() 
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Ask readline to retrieve console size */
  rl_resize_terminal();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* A prompt need some update to fit correctly on readline (with color) */
int		vm_rl_update_prompt(char *ptr, int size)
{
  int		i, tmpi;
  char		tmp[size];
#if defined(RL_PROMPT_START_IGNORE)
  int		open = 0;
#endif

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If no color on the prompt */
  if (strchr(ptr, C_STARTCOLOR) == NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  tmp[0] = '\0';

  /* Parse every char */
  for (i = 0, tmpi = 0; i < size && tmpi < size && ptr[i]; i++)
    {
      /* A color will always start with C_STARTCOLOR */
      if (ptr[i] == C_STARTCOLOR)
	{
#if defined(RL_PROMPT_START_IGNORE)
	  /* This work only with readline >= 4.x, damn you if you don't have it !!
	     We will use RL_PROMPT_START_IGNORE & RL_PROMPT_END_IGNORE that indicate 
	     to readline that chars (here color) must be ignored in count procedure */
	  tmp[tmpi++] = RL_PROMPT_START_IGNORE;
	  open++;

	  while(ptr[i] != 'm' && i < size && tmpi < size)
	      tmp[tmpi++] = ptr[i++];

	  if (i < size && ptr[i])
	    {
	      tmp[tmpi++] = ptr[i++];
	      tmp[tmpi++] = RL_PROMPT_END_IGNORE;
	      open--;
	    }
#else
	  /* If you don't have the right version, I strip ! */
	  while(ptr[i] != 'm' && i < size)
	    i++;
	  
	  if (i < size)
	    i++;
#endif

	  if (i >= size || ptr[i] == '\0')
	    break;

	  /* Support followed tags */
	  if (ptr[i] == C_STARTCOLOR)
	    {
	      i--;
	      continue;
	    }
	}

      tmp[tmpi++] = ptr[i];
    }

#if defined(RL_PROMPT_START_IGNORE)
  /* We didn't close an ignore sign */
  if (open > 0)
    tmp[(tmpi < size ? tmpi : size - 1)] = RL_PROMPT_END_IGNORE;
#endif

  /* Add the last char */
  tmp[(tmpi < size ? tmpi : size)] = '\0';

  /* Update the submited variable */
  memcpy(ptr, tmp, size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

#endif

