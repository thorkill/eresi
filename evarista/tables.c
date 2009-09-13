/**
* @file evarista/tables.c
 * @ingroup evarista
 *
 * This file contains all command and objects definitions for specific Evarista scripting
 *
 * Started on Fri Jun 22 07:48:41 2007 jfv
 * $Id$
 *
 */
#include "evarista.h"


/**
 * Setup the command hash table
 * @ingroup evarista
 */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  //revm_command_add(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);
}
