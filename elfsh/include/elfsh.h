/**
 * @file elfsh.h
 * @ingroup elfsh
 * 
 * Started on  Thu Feb 22 07:19:04 2001 jfv
 *
 *
 * $Id$
 *
 */
#ifndef __ELFSH_H_
 #define __ELFSH_H_

/* User defined configuration */
#include "libstderesi.h"

/* Help strings */
#include "elfsh-help.h"

/* Constants */
#define	ELFSH_NAME		"ELF shell"
#define	ELFSH_SNAME		"elfsh"
#define ELFSH_CONFIG		".elfshrc"

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
int	esh_main(int ac, char **av);

/* Commands registration */
void	setup_local_cmdhash();

#endif /* __ELFSH_H_ */

