/*
** tables.c for evarista in ERESI
**
** This file contains all command and objects definitions for specific Evarista scripting
**
** Started on Fri Jun 22 07:48:41 2007 jfv
** $Id: tables.c,v 1.3 2007-07-31 03:28:46 may Exp $
**
*/
#include "evarista.h"


/* Setup the command hash table */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  //revm_command_add(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);
}
