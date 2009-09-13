/**
* @file e2dbg/include/e2dbg.h
 * 
 * Started on  Thu Feb 22 07:19:04 2001 jfv
 *
 * $Id$
 *
 */
#ifndef __E2DBG_H_
#define __E2DBG_H_

/* User defined configuration */
#include "revm.h"

#define E2DBG_CONFIG ".e2dbgrc"

#if defined(__FreeBSD__) || defined(sun) || defined(__NetBSD__) || defined(__OpenBSD__)
  extern char **environ;
#endif

/* Top skeleton functions */
int	revm_main(int ac, char **av);

#endif /* __E2DBG_H_ */

