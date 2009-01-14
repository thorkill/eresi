/**
 * @file state.c
 * @ingroup libelfsh 
 * @brief Provide initialization and debugger state related functions.
 *
 * Started on  Sat Jun  2 15:20:18 2005 jfv
 * $Id$
 */
#include "libelfsh.h"


libworld_t	dbgworld;


void    elfsh_init() __attribute__ ((constructor));


/**
 * @brief Initialize the elfsh world
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
 * @brief The functions for simple ondisk/memory state flag 
 */
u_char	elfsh_is_static_mode()
{
  return (dbgworld.mode == LIBELFSH_MODE_STATIC);
}

/**
 * @brief Set static mode
 */
void	elfsh_set_static_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_STATIC);
}


/**
 * @ brief check if we are running in debug mode
 */
Bool	elfsh_is_runtime_mode()
{
 return (dbgworld.mode == LIBELFSH_MODE_RUNTIME);
}

/** 
 * @brief set debug mode
 */
void	elfsh_set_runtime_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_RUNTIME);
  dbgworld.indebug = 1;
}

/**
 * @brief set mode
 * @param mode
 */
void	elfsh_set_mode(u_char mode)
{
  //fprintf(stderr, "MODE changed for [%s] ! \n",
  //(mode == LIBELFSH_MODE_STATIC ? "static" : "dynamic"));
  dbgworld.mode = mode;
}

/**
 * @brief get mode
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
 * @brief toggle modes
 */

void	elfsh_toggle_mode()
{
  if (elfsh_is_runtime_mode())
    elfsh_set_static_mode();
  else
    elfsh_set_runtime_mode();
}

/**
 * @brief Print last error
 */
void	elfsh_error()
{
  profiler_error();
}
