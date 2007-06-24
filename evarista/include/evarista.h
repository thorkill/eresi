/*
** evarista.h for evarista in ERESI
** 
** Started on Fri Jun 22 14:19:04 2007 mayhem
** $Id: evarista.h,v 1.1 2007-06-24 14:39:12 may Exp $
*/
#ifndef __EVARISTA_H_
 #define __EVARISTA_H_

/* User defined configuration */
#include "revm.h"

/* Help strings */
#include "evarista-help.h"

/* Top skeleton functions */
int	eva_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __EVARISTA_H_ */

