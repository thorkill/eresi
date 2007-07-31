/*
** evarista.h for evarista in ERESI
** 
** Started on Fri Jun 22 14:19:04 2007 jfv
** $Id: evarista.h,v 1.3 2007-07-31 03:28:46 may Exp $
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

