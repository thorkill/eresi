/*
** state.c for elfsh
** 
** Started on  Sat Jun  2 15:20:18 2005 mayhem
**
*/
#include "libelfsh.h"


libworld_t	dbgworld;


void	elfsh_init() __attribute__ ((constructor));

void	elfsh_init()
{
  memset(&dbgworld, 0x00, sizeof(dbgworld));
  elfsh_set_static_mode();
}

/* The functions for simple ondisk/memory state flag */
u_char	elfsh_is_static_mode()
{
  return (dbgworld.mode == LIBELFSH_MODE_STATIC);
}

void	elfsh_set_static_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_STATIC);
}

u_char	elfsh_is_debug_mode()
{
 return (dbgworld.mode == LIBELFSH_MODE_E2DBG);
}

void	elfsh_set_debug_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_E2DBG);
  dbgworld.indebug = 1;
}


void	elfsh_set_mode(u_char mode)
{
  dbgworld.mode = mode;
}

u_char	elfsh_get_mode()
{
  return (dbgworld.mode);
}

u_char	elfsh_debugger_present()
{
  return (dbgworld.indebug);
}


/* Here the function for the profiler buffer */
int	elfsh_prof_enable()
{
  dbgworld.prof_enable = 1;
  return (0);
}

int	elfsh_prof_disable()
{
  dbgworld.prof_enable = 0;
  return 0;
}

int	elfsh_is_prof_enable()
{
  return (dbgworld.prof_enable);
}

/* Change the profiling output function */
/* Usually vm_output located in elfsh VM */
void	elfsh_set_profile(int (*profile)(char *))
{
  dbgworld.profile = profile;
}
