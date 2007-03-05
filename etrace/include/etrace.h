/*
** etrace.h for etrace
** 
** Started on  Thu Feb 22 07:19:04 2001 mxatone
*/
#ifndef __ETRACE_H_
 #define __ETRACE_H_

/* User defined configuration */
#include "revm.h"

/* Help strings */
#include "etrace-help.h"

#define ETRACE_SNAME	       	"etrace"
#define ETRACE_NAME		"Etrace shell"
#define ETRACE_VERSION 		"0.1"
#define	ETRACE_RELEASE		"a"
#define ETRACE_EDITION		"dev"


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
int	vm_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __ETRACE_H_ */

