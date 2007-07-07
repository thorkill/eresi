/*
** evarista.h for evarista in ERESI
** 
** Started on Fri Jun 22 14:19:04 2007 mayhem
** $Id: evarista.h,v 1.2 2007-07-07 17:30:24 may Exp $
*/
#ifndef __EVARISTA_H_
 #define __EVARISTA_H_

/* User defined configuration */
#include "revm.h"

/* Help strings */
#include "evarista-help.h"

#define EVARISTA_NAME	       	"Evarista static analyzer"
#define	EVARISTA_SNAME		"evarista"

/* Top skeleton functions */
int	eva_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __EVARISTA_H_ */

