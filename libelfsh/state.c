/*
** state.c for elfsh
** 
** Started on  Sat Jun  2 15:20:18 2005 mayhem
** 
**
** $Id: state.c,v 1.12 2007-05-16 23:36:37 may Exp $
**
*/
#include "libelfsh.h"


libworld_t	dbgworld;


void    elfsh_init() __attribute__ ((constructor));


/**
 * Initialize the elfsh world
 */
void    elfsh_init()
{
  memset(&dbgworld, 0x00, sizeof(dbgworld));

  aspect_called_ctors_inc();

  /* Initialize the aspect library and set ondisk mode by default */
  aspect_init();
  elfsh_set_static_mode();

  if (aspect_called_ctors_finished())
    e2dbg_presence_reset();

}


/**
 * The functions for simple ondisk/memory state flag 
 */
u_char	elfsh_is_static_mode()
{
  return (dbgworld.mode == LIBELFSH_MODE_STATIC);
}

/**
 * Set static mode
 */
void	elfsh_set_static_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_STATIC);
}


/**
 * check if we are running in debug mode
 */
u_char	elfsh_is_debug_mode()
{
 return (dbgworld.mode == LIBELFSH_MODE_E2DBG);
}

/** 
 * set debug mode
 */
void	elfsh_set_debug_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_E2DBG);
  dbgworld.indebug = 1;
}

/**
 * set mode
 */
void	elfsh_set_mode(u_char mode)
{
  //fprintf(stderr, "MODE changed for [%s] ! \n",
  //(mode == LIBELFSH_MODE_STATIC ? "static" : "dynamic"));

  dbgworld.mode = mode;
}

/**
 * get mode
 */
u_char	elfsh_get_mode()
{
  return (dbgworld.mode);
}

/**
 * check if we are in debug
 */ 
u_char	elfsh_debugger_present()
{
  return (dbgworld.indebug);
}


/**
 * toggle modes
 */

void	elfsh_toggle_mode()
{
  if (elfsh_is_debug_mode())
    elfsh_set_static_mode();
  else
    elfsh_set_debug_mode();
}

/**
 * Print last error
 */
void	elfsh_error()
{
  profiler_error();
}
