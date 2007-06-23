/*
** tables.c for etrace
**
** This file contains all command and objects definitions for scripting
**
** Started on  Sat Jan 25 07:48:41 2003 mxatone
**
** $Id: tables.c,v 1.5 2007-06-23 17:11:00 mxatone Exp $
**
*/
#include "etrace.h"


/* Setup the command hash table */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  if (world.state.vm_mode != REVM_STATE_CMDLINE)
    vm_addcmd(CMD_RUN , (void *) cmd_run   , (void *) vm_getvarparams, 0, HLP_RUN);
  vm_addcmd(CMD_STRIP , (void *) cmd_strip , (void *) NULL           , 1, HLP_STRIP);
  vm_addcmd(CMD_SSTRIP, (void *) cmd_sstrip, (void *) NULL           , 1, HLP_SSTRIP);
  vm_addcmd(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);

  /* Trace commands */
  vm_addcmd(CMD_TRACES      , (void *) cmd_traces      , (void *) vm_getvarparams , 1, HLP_TRACES);
  vm_addcmd(CMD_TRACEADD    , (void *) cmd_traceadd    , (void *) vm_getvarparams , 1, HLP_TRACEADD);
  vm_addcmd(CMD_TRACEEXCLUDE, (void *) cmd_traceexclude, (void *) vm_getvarparams , 1, HLP_TRACEEXCLUDE);;
  vm_addcmd(CMD_TRACERUN    , (void *) cmd_tracerun    , (void *) vm_getvarparams , 1, HLP_TRACERUN);

  if (world.state.vm_mode == REVM_STATE_TRACER)
    {
      /* Traces aliases */
      vm_addcmd(CMD_TRACEADD_CMDLINE , (void *) cmd_traceadd    , (void *) vm_getvarparams, 1, HLP_TRACEADD);
      vm_addcmd(CMD_TRACEEXCL_CMDLINE, (void *) cmd_traceexclude, (void *) vm_getvarparams, 1, HLP_TRACEEXCLUDE);
    }
}
