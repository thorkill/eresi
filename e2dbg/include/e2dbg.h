/*
** e2dbg.h for elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 jfv
**
** $Id: e2dbg.h,v 1.23 2007-07-31 03:28:46 may Exp $
**
*/
#ifndef __E2DBG_H_
 #define __E2DBG_H_

/* User defined configuration */
#include "revm.h"

#if defined(__FreeBSD__) || defined(sun) || defined(__NetBSD__)
  extern char **environ;
#endif

/* Top skeleton functions */
int	revm_main(int ac, char **av);

#endif /* __E2DBG_H_ */

