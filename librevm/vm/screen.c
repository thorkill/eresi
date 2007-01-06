/*
** screen.c for librevm in elfsh
**
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Thu Jan 04 11:22:11 2007 mayhem
*/
#include "revm.h"


int		vm_clearscreen(int i, char c)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(world.curjob->screen.buf);
  world.curjob->screen.head = world.curjob->screen.tail = NULL;
  world.curjob->screen.buf = NULL;

#if defined(USE_READLN)
  if (rl_ctrll)
    rl_ctrll(i, c);
  world.curjob->io.buf = NULL;
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		vm_install_clearscreen()
{
#if defined(USE_READLN)
  Keymap	    map;
#endif
  char		    keyseq[2];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(USE_READLN)
  map = rl_get_keymap();
  keyseq[0] = CTRL('l');
  keyseq[1] = '\0';
  rl_ctrll = rl_function_of_keyseq(keyseq, map, NULL);
  rl_bind_key(CTRL('l'), (rl_command_func_t *) vm_clearscreen);
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Update the screen depending of the actual job */
int		vm_screen_update(u_short new, u_short prompt_display)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* clear screen */
  vm_output_nolog("\033[2J");
  vm_output_nolog("\033[0;0H");

#if defined(USE_READLN)
  if (new)
    world.curjob->io.buf = NULL;
#endif

  vm_flush();

  /* Rebuild the current screen */
  if (world.curjob->screen.buf != NULL)
    {
      vm_output_nolog(world.curjob->screen.head);
      if (world.curjob->screen.head >= world.curjob->screen.tail)
	{
	  vm_output_nolog(world.curjob->screen.head);
	  vm_output_nolog(world.curjob->screen.buf);
	}
    }

#if defined (USE_READLN)
  if (new)
    {
      /* Setup on new screen */
      vm_log(vm_get_prompt());
      rl_on_new_line();

      world.curjob->io.savebuf = NULL;
    }
  else
    {
      /* Prompt already here */
      if (!prompt_display)
	rl_on_new_line_with_prompt();
      rl_refresh_line(0, 0);
    }

  rl_clear_message();

  if (!new)
    {
      /* Restore the buffer */
      if (world.curjob->io.savebuf)
	strcpy(rl_line_buffer, world.curjob->io.savebuf);
      rl_point = world.curjob->io.rl_point;
      rl_end = world.curjob->io.rl_end;
    }

  rl_redisplay();
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int		vm_screen_switch()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (cmd_next_workspace() <= 0)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, vm_screen_update(0, 1));
}

