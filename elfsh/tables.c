/*
** tables.c for elfsh
**
** This file contains all command and objects definitions for scripting
**
** Started on  Sat Jan 25 07:48:41 2003 mayhem
**
** $Id: tables.c,v 1.16 2007-07-17 18:11:24 may Exp $
**
*/
#include "elfsh.h"


/* Setup the command hash table */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    revm_command_add(CMD_RUN , (void *) cmd_run   , (void *) revm_getvarparams, 0, HLP_RUN);
  revm_command_add(CMD_STRIP , (void *) cmd_strip , (void *) NULL           , 1, HLP_STRIP);
  revm_command_add(CMD_SSTRIP, (void *) cmd_sstrip, (void *) NULL           , 1, HLP_SSTRIP);
  revm_command_add(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);
}
