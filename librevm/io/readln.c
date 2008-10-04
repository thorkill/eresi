/**
 * @file readln.c
 * @ingroup io
 * @brief Interface readline with librevm.
 *
 * This allows us to only recompile this file
 * when the user recompiles enabling/disabling readline.
 *
 * Started on  Fri Mar  5 00:55:40 2004 jfv
 * $Id: readln.c,v 1.2 2007-11-29 15:33:39 may Exp $
 */
#include "revm.h"

/**
 * Reset lines counters and ignore output state 
 */
void		revm_ctrl_set(int i, char c)
{
#if defined(USE_READLN)
  readln_ctrl_set(i, c);
  world.curjob->ws.io.buf = NULL;
#endif
}

void		revm_buffer_reset(u_short isnew)
{
#if defined(USE_READLN)
  if (isnew)
    world.curjob->ws.io.buf = NULL;
#endif
}

void            revm_screen_change(u_short isnew, char promptdisplay)
{
#if defined(USE_READLN)
  readln_screen_change(isnew, promptdisplay);
#endif
}

int             revm_prompt_restore()
{
#if defined(USE_READLN)
  return (readln_prompt_restore());
#else
  return (0);
#endif
}

int             revm_prompt_update(char *ptr, int size)
{
#if defined(USE_READLN)
  return (readln_prompt_update(ptr, size));
#else
  return (0);
#endif
}

char            *revm_input_check()
{
#if defined(USE_READLN)
  return (readln_input_check());
#else
  return (NULL);
#endif
}

int             revm_init_history(int mode)
{
#if defined(USE_READLN)
  return (readln_init(mode, (char *)config_get_data(ERESI_VMCONFIG_HISTORY)));
#else
  return (0);
#endif
}

int             revm_quit(int mode)
{
#if defined(USE_READLN)
  return (readln_quit(mode, (char *)config_get_data(ERESI_VMCONFIG_HISTORY)));
#else
  return (0);
#endif
}

void            revm_completion_install(char mode, char side)
{
#if defined(USE_READLN)
  readln_completion_install(mode, side);
#endif
}

void            revm_input_log(char *str)
{
#if defined(USE_READLN)
  readln_input_log(str);
#endif
}

void            revm_completion_commands(hash_t *cmd_hash)
{
#if defined(USE_READLN)
  readln_completion_commands(cmd_hash);
#endif
}

int             revm_column_update()
{
#if defined(USE_READLN)
  return (readln_column_update());
#else
  return (0);
#endif
}

void            revm_terminal_prepare(char mode)
{
#if defined(USE_READLN)
  readln_terminal_prepare(mode);
#endif
}

void            revm_terminal_unprepare(char mode)
{
#if defined(USE_READLN)
  readln_terminal_unprepare(mode);
#endif
}

void		revm_job_preswitch()
{
#if defined(USE_READLN)
  if (world.curjob->ws.io.savebuf)
    XFREE(__FILE__, __FUNCTION__, __LINE__,world.curjob->ws.io.savebuf);
  world.curjob->ws.io.savebuf = strdup(rl_line_buffer); 
  world.curjob->ws.io.buf = NULL;
  world.curjob->ws.io.rl_point = rl_point;
  world.curjob->ws.io.rl_end = rl_end;
#endif
}

void		revm_job_postswitch()
{
#if defined(USE_READLN)
  rl_set_prompt(revm_get_prompt());
#endif
}

void		revm_screen_getsize(int *lines, int *cols)
{
#if defined(USE_READLN)
  rl_get_screen_size(lines, cols);
#endif
}


void		revm_buffer_free(char *buf)
{
#if !defined(USE_READLN)
  NOPROFILER_IN();
  XFREE(__FILE__, __FUNCTION__, __LINE__, buf);
  NOPROFILER_OUT();
#endif
}


/**
 * @brief  Strip readline escape characters from buffer
 * @ingroup io
 */
void		revm_strip_char(char *str, char c)
{
#if defined(USE_READLN) && defined(RL_PROMPT_START_IGNORE)
  unsigned int		len, pos;
  char		*search;

  NOPROFILER_IN();
  len = strlen(str);
  for (search = str; (search = strchr(search, c)) != NULL;)
    {
      pos = search - str;
      /* Realign others */
      memmove(search, search+1, len - (pos + 1));
      len--;
    }
  NOPROFILER_OUT();
#endif
}


void		revm_prompt_postselect_restore(fd_set *sel_sockets)
{  
#if defined (USE_READLN)
  if (world.state.revm_side == REVM_SIDE_CLIENT && FD_ISSET(0, sel_sockets))
    readln_prompt_restore();
#endif
}

void		revm_callback_handler_remove()
{
#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif
}

void		revm_callback_handler_install(char *prompt, void (*fct)(char *str))
{
#if defined(USE_READLN)
  rl_callback_handler_install(prompt, fct);
#endif
}



void		revm_prompt_log()
{
#if defined (USE_READLN)
  revm_log(revm_get_prompt());
#endif
}


void		revm_conditional_rlquit()
{
#if defined(USE_READLN)
  if (!(world.state.revm_mode == REVM_STATE_DEBUGGER
	&& world.state.revm_side == REVM_SIDE_SERVER))
    readln_quit(world.state.revm_mode, (char *)config_get_data(ERESI_VMCONFIG_HISTORY));
#endif
}

void		revm_input_prelog(char *buf)
{
#if defined(USE_READLN)
  revm_input_log(buf);
#else
  revm_log(buf);
  revm_log("\n\n");
#endif
}

void		revm_rlfifo_write()
{
#if defined(USE_READLN)
  write(world.fifo_c2s, "\n", 1);
#endif
}

int		revm_is_enabled()
{
#if defined(USE_READLN)
  return (1);
#else
  return (0);
#endif
}


int		revm_is_stdinput()
{
#if defined(USE_READLN)
  if (world.curjob->ws.io.type == REVM_IO_STD)
    {
      if (world.curjob->ws.io.buf != NULL) 
	{
	  /* On the client side, we consider that the prompt is already
	     returned by the server */
	  if (world.state.revm_mode == REVM_STATE_DEBUGGER &&
	      world.state.revm_side == REVM_SIDE_CLIENT)
	    {
	      rl_on_new_line_with_prompt();
	      rl_clear_message();
	      //rl_redisplay();
	    }
	  else
	    rl_forced_update_display();
	  revm_log(revm_get_prompt());
	}
      return (0);
    }
  else 
    return (1);
#else
  return (1);
#endif
}

void          revm_line_handler(char *c)
{
#if defined(USE_READLN)
  readln_ln_handler(c);
#endif
}
