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


void	elfsh_toggle_mode()
{
  if (elfsh_is_debug_mode())
    elfsh_set_static_mode();
  else
    elfsh_set_debug_mode();
}



/* Here the functions for the profiler option */
int	elfsh_prof_enable_err()
{
  dbgworld.proflevel = ELFSH_ERRPROF;
  return (0);
}

int	elfsh_prof_enable_out()
{
  dbgworld.proflevel = ELFSH_OUTPROF;
  return (0);
}

int	elfsh_prof_disable()
{
  dbgworld.proflevel = ELFSH_NOPROF;
  return 0;
}

int	elfsh_is_prof_enable()
{
  return (dbgworld.proflevel);
}

/* Change the profiling output function */
/* Usually vm_output located in elfsh VM */
void	elfsh_set_profile(int (*profile)(char *), int (*profile_err)(char *))
{
  dbgworld.profile = profile;
  dbgworld.profile_err = profile_err;

}
