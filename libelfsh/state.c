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
  //fprintf(stderr, "Changed for %s mode ! \n",
  //(mode == LIBELFSH_MODE_STATIC ? "static" : "dynamic"));
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

/* We setup two functions for colors because we have too many functions */

/* Change simple color functions */
void	elfsh_set_color_simple(void (*endline)(), char *(*colorinstr)(char *text),
			       char *(*colorstr)(char *t), char *(*colorfieldstr)(char *t),
			       char *(*colortypestr)(char *t), char *(*colorend)(char *text),
			       char *(*colorwarn)(char *text))
{
  dbgworld.endline = endline;
  dbgworld.colorinstr = colorinstr;
  dbgworld.colorstr = colorstr;
  dbgworld.colorfieldstr = colorfieldstr;
  dbgworld.colortypestr = colortypestr;
  dbgworld.colorend = colorend;
  dbgworld.colorwarn = colorwarn;
}

/* Change advanced color functions */
void	elfsh_set_color_advanced(char *(*coloradv)(char *ty, char *p, char *te),
				 char *(*colorinstr_fmt)(char* p, char *t),
				 char *(*coloraddress)(char *p, elfsh_Addr a),
				 char *(*colornumber)(char *p, u_int n),
				 char *(*colorstr_fmt)(char *p, char *t),
				 char *(*colorfieldstr_fmt)(char *p, char *t),
				 char *(*colortypestr_fmt)(char *p, char *t),
				 char *(*colorwarn_fmt)(char *pattern, char *text))
{
  dbgworld.coloradv = coloradv;
  dbgworld.colorinstr_fmt = colorinstr_fmt;
  dbgworld.coloraddress = coloraddress;
  dbgworld.colornumber = colornumber;
  dbgworld.colorstr_fmt = colorstr_fmt;
  dbgworld.colorfieldstr_fmt = colorfieldstr_fmt;
  dbgworld.colortypestr_fmt = colortypestr_fmt;
  dbgworld.colorwarn_fmt = colorwarn_fmt;
}
