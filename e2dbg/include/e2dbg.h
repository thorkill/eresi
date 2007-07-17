/*
** e2dbg.h for elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 mayhem
**
** $Id: e2dbg.h,v 1.22 2007-07-17 18:11:24 may Exp $
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

