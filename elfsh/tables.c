/*
** tables.c for elfsh
**
** This file contains all command and objects definitions for scripting
**
** Started on  Sat Jan 25 07:48:41 2003 mayhem
*/
#include "elfsh.h"


/* Setup the command hash table */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  if (world.state.vm_mode != REVM_STATE_CMDLINE)
    vm_addcmd(CMD_RUN , (void *) cmd_run   , (void *) vm_getvarparams, 0, HLP_RUN);
  vm_addcmd(CMD_STRIP , (void *) cmd_strip , (void *) NULL           , 1, HLP_STRIP);
  vm_addcmd(CMD_SSTRIP, (void *) cmd_sstrip, (void *) NULL           , 1, HLP_SSTRIP);
  vm_addcmd(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);
}
