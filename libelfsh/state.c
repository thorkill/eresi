/**
 * @file state.c
 * @ingroup libelfsh 
 * @brief Provide initialization and debugger state related functions.
 *
 * Started on  Sat Jun  2 15:20:18 2005 jfv
 * 
 *
 * $Id: state.c,v 1.17 2008-02-16 12:32:27 thor Exp $
 *
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
unsigned char	elfsh_is_static_mode()
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
unsigned char	elfsh_is_debug_mode()
{
 return (dbgworld.mode == LIBELFSH_MODE_E2DBG);
}

/** 
 * @brief set debug mode
 */
void	elfsh_set_debug_mode()
{
  elfsh_set_mode(LIBELFSH_MODE_E2DBG);
  dbgworld.indebug = 1;
}

/**
 * @brief set mode
 * @param mode
 */
void	elfsh_set_mode(unsigned char mode)
{
  //fprintf(stderr, "MODE changed for [%s] ! \n",
  //(mode == LIBELFSH_MODE_STATIC ? "static" : "dynamic"));

  dbgworld.mode = mode;
}

/**
 * @brief get mode
 */
unsigned char	elfsh_get_mode()
{
  return (dbgworld.mode);
}

/**
 * check if we are in debug
 */ 
unsigned char	elfsh_debugger_present()
{
  return (dbgworld.indebug);
}


/**
 * @brief toggle modes
 */

void	elfsh_toggle_mode()
{
  if (elfsh_is_debug_mode())
    elfsh_set_static_mode();
  else
    elfsh_set_debug_mode();
}

/**
 * @brief Print last error
 */
void	elfsh_error()
{
  profiler_error();
}
