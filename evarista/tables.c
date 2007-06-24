/*
** tables.c for evarista in ERESI
**
** This file contains all command and objects definitions for specific Evarista scripting
**
** Started on Fri Jun 22 07:48:41 2007 mayhem
** $Id: tables.c,v 1.1 2007-06-24 14:38:50 may Exp $
**
*/
#include "evarista.h"


/* Setup the command hash table */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  //vm_addcmd(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);
}
