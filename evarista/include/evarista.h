/**
 * @defgroup evarista Evarista: static analyzer
 * @file evarista/include/evarista.h
 * 
 * Started on Fri Jun 22 14:19:04 2007 jfv
 * $Id$
 */
#ifndef __EVARISTA_H_
 #define __EVARISTA_H_

/* User defined configuration */
#include "revm.h"

/* Help strings */
#include "evarista-help.h"

#define EVARISTA_NAME	       	"Evarista static analyzer"
#define	EVARISTA_SNAME		"evarista"
#define EVARISTA_CONFIG         ".evaristarc"

/* Top skeleton functions */
int	eva_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __EVARISTA_H_ */

