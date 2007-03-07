/*
** e2dbg.h for elfsh
** 
** Started on  Thu Feb 22 07:19:04 2001 mayhem
**
** $Id: e2dbg.h,v 1.19 2007-03-07 16:45:34 thor Exp $
**
*/
#ifndef __E2DBG_H_
 #define __E2DBG_H_

/* User defined configuration */
#include "revm.h"

#if defined(__FreeBSD__)
  extern char **environ;
#endif

/* Top skeleton functions */
int	vm_main(int ac, char **av);

#endif /* __E2DBG_H_ */

