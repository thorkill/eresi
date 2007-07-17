/*
** etrace.h for etrace
** 
** Started on  Thu Feb 22 07:19:04 2001 mxatone
**
** $Id: etrace.h,v 1.4 2007-07-17 18:11:24 may Exp $
**
*/
#ifndef __ETRACE_H_
 #define __ETRACE_H_

/* User defined configuration */
#include "revm.h"

/* Help strings */
#include "etrace-help.h"

#define ETRACE_NAME	       	"Embedded ELF Tracer"
#define	ETRACE_SNAME		"etrace"

/* Commands */
#define	CMD_STRIP		"strip"
#define	CMD_SSTRIP		"sstrip"
#define	CMD_CLEANUP		"cleanup"
#define	CMD_SHTRM		"shtrm"
#define	CMD_RUN			"run"

/* Commands execution handlers, each in their respective file */
int	cmd_strip();
int	cmd_sstrip();
int	cmd_cleanup();
int	cmd_run();

/* Top skeleton functions */
int	revm_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __ETRACE_H_ */

