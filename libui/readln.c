/*
** @file readln.c
**
** Started on  Tue Feb 18 06:24:42 2003 emsi
** Updated on  Fri Feb 18 23:59:25 2006 thorkill
** Updated on  Tue Jun 27 23:51:04 2006 mxatone
**
** $Id: readln.c,v 1.20 2007-11-28 07:56:09 may Exp $
**
*/
#include "libui.h"

#if defined(USE_READLN)

rlcomp_t		comp;			/* Completion strings */
rl_command_func_t	*rl_ctrll = NULL;
char			readln_exited = 0;

/**
 * @brief Clean readline 
 */
int		readln_quit(int mode)
{
  int 		exited = readln_exited;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (readln_exited == 0)
    {
      if (mode >= 0)
	readln_history_dump(mode);
      
      rl_callback_handler_remove();
      readln_exited = 1;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, exited);
}


/**
 * @brief Set rl_ctrl 
 */
void		readln_ctrl_set(int i, char c)
{
  if (rl_ctrll)
    rl_ctrll(i, c);
}

/** 
 * @brief Completion fonction for the command list 
 */
char		*readln_match(const char *text, int state)
{
  static int	i, len, tab;
  char		*name, *name2;
  const char    *baq;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
    if (comp.cmds[tab] != NULL)
      for (; comp.cmds[tab][i] != NULL; i++)
	{
	  name = comp.cmds[tab][i];
	  if (!strncmp(name, text, len))
	    {
	      XALLOC(__FILE__, __FUNCTION__, __LINE__,
		     name2, strlen(name) + 1, NULL);
	      strcpy(name2, name);
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, name2);
	    }
	}

  /* If no names matched, then return NULL. */
  i = tab = len = 0;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  ((char *) NULL));
}



/** 
 * @brief Add commands to completion 
 */
void		readln_completion_commands(hash_t *cmd_hash)
{
  comp.cmds[0] = hash_get_keys(cmd_hash, NULL);
}


/** 
 * @brief Install the completion strings 
 */
void		readln_completion_install(char mode, char side)
{
  char		*str;
  FILE		*fifofile;

  //fprintf(stderr, "Installing completion now \n");
  
  comp.cmds[0]  = hash_get_keys(&cmd_hash    , NULL);
  comp.cmds[1]  = hash_get_keys(&exprs_hash  , NULL);
  comp.cmds[2]  = hash_get_keys(&const_hash  , NULL);
  comp.cmds[3]  = hash_get_keys(&mod_hash    , NULL);
  comp.cmds[4]  = hash_get_keys(&L1_hash     , NULL);
  comp.cmds[5]  = hash_get_keys(&elf_L2_hash , NULL);
  comp.cmds[6]  = hash_get_keys(&sht_L2_hash , NULL);
  comp.cmds[7]  = hash_get_keys(&pht_L2_hash , NULL);
  comp.cmds[8]  = hash_get_keys(&sym_L2_hash , NULL);
  comp.cmds[9]  = hash_get_keys(&rel_L2_hash , NULL);
  comp.cmds[10] = hash_get_keys(&dynsym_L2_hash, NULL);
  comp.cmds[11] = hash_get_keys(&dyn_L2_hash , NULL);
  comp.cmds[12] = hash_get_keys(&sct_L2_hash , NULL);
  comp.cmds[13] = hash_get_keys(&fg_color_hash, NULL);
  comp.cmds[14] = hash_get_keys(&t_color_hash, NULL);
  comp.cmds[15] = NULL;
  
  using_history();
  rl_attempted_completion_function = readln_completion;

  str = "";
  if (mode != REVM_STATE_DEBUGGER && mode != REVM_STATE_CMDLINE 
      && mode != REVM_STATE_SCRIPT && mode != REVM_STATE_TRACER)
    str = revm_get_prompt();

  rl_callback_handler_install(str, revm_ln_handler);

  /* We setup prompt after installation because we
   don't want it directly */
  if (mode == REVM_STATE_DEBUGGER && side == REVM_SIDE_CLIENT)
    {
      rl_on_new_line_with_prompt();
      rl_prompt = revm_get_prompt();
    }

  rl_bind_key(CTRL('x'), revm_screen_switch);
  readln_install_clearscreen();
  readln_column_update();
  signal(SIGWINCH, (void *) readln_column_update);
  rl_catch_sigwinch = 0;
  rl_set_signals();
}



/** 
 * @brief Perform completion 
 */
char	**readln_completion(const char* text, int start, int end) 
{
  char	**matches = (char**) NULL;
  char	*baq, *baq2;
  char	buf[50];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /*
    fprintf(stderr, "Calling completion on %s \n", text);
    prevent freeing of unitialized memory on FreeBSD
    XXX: check this !!
   */

  if (strlen(text) == 0)
   PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

#if defined(__OpenBSD__)
  matches = completion_matches(text, readln_match);
#else
  matches = rl_completion_matches(text, readln_match);
#endif

  if (!matches)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);

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
      //seems not to be free
      //XFREE(__FILE__, __FUNCTION__, __LINE__,matches[0]);
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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, matches);
}


/**
 * This function was used to update columns on a readline colored prompt
 *  another solution has been found that work well (see readln_prompt_update).
 *  We keep this function because it can be useful later. 
 */
int 	readln_column_update() 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Ask readline to retrieve console size */
  rl_resize_terminal();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * @brief A prompt need some update to fit correctly on readline (with color) 
 */
int		readln_prompt_update(char *ptr, int size)
{
  int		i, tmpi;
  char		tmp[size];
#if defined(RL_PROMPT_START_IGNORE)
  int		open = 0;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If no color on the prompt */
  if (strchr(ptr, C_STARTCOLOR) == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/** 
 * @brief readline line handler 
 */
void    revm_ln_handler(char *c)
{
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  
  world.curjob->ws.io.buf = c;

  /* save (remove) prompt if a complete line was typed
   * so that the line is not displayed on return */
  if (c != NULL)
    {
      if (!c[0])
	revm_log("\n");
    }

  /* special to enable exit on CTRL-D */
  else
    world.curjob->ws.io.buf = (char *) REVM_INPUT_EXIT;

  rl_save_prompt();
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Restore readline prompt that will be display on next 
 * rl_forced_update_display() or rl_callback_read_char() 
 */
int		readln_prompt_restore()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->ws.io.buf = NULL;  
  rl_callback_read_char();
  
  if (world.curjob->ws.io.buf != NULL)
    {
      rl_restore_prompt();
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Perform some checks on the input given by readline 
 */
char		*readln_input_check()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* input in progress */
  if (world.curjob->ws.io.buf == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		  ((char *) REVM_INPUT_VOID));
  
  /* CTRL-D case */
  if (world.curjob->ws.io.buf == (char *) REVM_INPUT_EXIT)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		  (NULL));
  
  /* empty string */
  if (strlen(world.curjob->ws.io.buf) == 0)
    {
      /* XXX memory leak, this dup will never be freed */
      if (world.curjob->ws.oldline)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (strdup(world.curjob->ws.oldline)));
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    ((char *) REVM_INPUT_VOID));
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		(strdup(world.curjob->ws.io.buf)));
}


/** 
 * Log the input of readline 
 */
void		readln_input_log(char *str)
{
#if 0
  HISTORY_STATE	*state;
  HIST_ENTRY	*entry;
  int		i;
#endif

  //if (world.curjob->io.buf[0] != '\0')
  if (*str)
    {
      revm_log(str);
      revm_log("\n\n");
    }
  else
    revm_log("\n");
  
#if 0
  printf ("[READLN] history : [%s]\n", buf);
  state = history_get_history_state();
  printf("== HISTORY STATE ==\n");
  printf("   offset : %d\n", state->offset);
  printf("   length : %d\n", state->length);
  printf("   size : %d\n", state->size);
  printf("===================\n");
#endif

  add_history(str);
  using_history();

#if 0
  state = history_get_history_state();
  printf("== HISTORY STATE ==\n");
  printf("   offset : %d\n", state->offset);
  printf("   length : %d\n", state->length);
  printf("   size : %d\n", state->size);
  printf("===================\n");
  printf("-- entries --\n");
  for (i = 0; i <= state->length + 1; i++)
    {
      entry = history_get(i);
      if (entry)
	printf("-- entry %d [%s] (%x) \n", i, entry->line, entry->line);
      else
	printf("-- entry %d NULL \n", i);
    }
  printf("-------------\n\n");
#endif
  
}




/** 
 * @brief Install the shortcut for clearing the screen 
 */
void		readln_install_clearscreen()
{
  Keymap	map;
  char		keyseq[2];

  map       = rl_get_keymap();
  keyseq[0] = CTRL('l');
  keyseq[1] = '\0';
  rl_ctrll  = rl_function_of_keyseq(keyseq, map, NULL);
  rl_bind_key(CTRL('l'), (rl_command_func_t *) revm_screen_clear);
}


/** 
 * @brief Change the screen content 
 */
void		readln_screen_change(u_short isnew, char prompt_display)
{
  /* Setup on new screen */
  if (isnew)
    {
      revm_log(revm_get_prompt());
      rl_on_new_line();
      world.curjob->ws.io.savebuf = NULL;
    }

  /* Prompt already here */
  else
    {
      if (!prompt_display)
	rl_on_new_line_with_prompt();
      rl_refresh_line(0, 0);
    }

  rl_clear_message();

  /* Restore the buffer */
  if (!isnew)
    {
      if (world.curjob->ws.io.savebuf)
	strcpy(rl_line_buffer, world.curjob->ws.io.savebuf);
      rl_point = world.curjob->ws.io.rl_point;
      rl_end = world.curjob->ws.io.rl_end;
    }

  rl_redisplay();
}


/** 
 * @brief Write readline history on quit 
 */
void		readln_history_dump(char mode)
{
  if (mode == REVM_STATE_INTERACTIVE || mode == REVM_STATE_DEBUGGER)
    {
      revm_output(" [*] Writting history (.elfsh_history) \n");
      write_history(".elfsh_history");
    }

  rl_callback_handler_remove();
}

/** 
 * @brief Prepare readline terminal 
 */
void		readln_terminal_prepare(char mode)
{
  if (mode == REVM_STATE_DEBUGGER || mode == REVM_STATE_INTERACTIVE)
    rl_prep_terminal(1);
}

/** 
 * @brief Prepare readline terminal 
 */
void		readln_terminal_unprepare(char mode)
{
  if (mode == REVM_STATE_DEBUGGER || mode == REVM_STATE_INTERACTIVE)
    rl_deprep_terminal();
}

#endif

