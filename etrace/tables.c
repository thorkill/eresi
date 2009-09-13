/**
 ** tables.c for etrace
 **
* @file etrace/tables.c
 ** @ingroup etrace
 ** @brief All command and objects definitions for scripting
 **
 ** Started on  Sat Jan 25 07:48:41 2003 mxatone
 ** $Id$
 */
#include "etrace.h"


/**
 * Setup the command hash table 
 */
void	setup_local_cmdhash()
{

  /* Interactive mode / Scripting mode commands */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    revm_command_add(CMD_RUN , (void *) cmd_run   , (void *) revm_getvarparams, 0, HLP_RUN);
  revm_command_add(CMD_STRIP , (void *) cmd_strip , (void *) NULL           , 1, HLP_STRIP);
  revm_command_add(CMD_SSTRIP, (void *) cmd_sstrip, (void *) NULL           , 1, HLP_SSTRIP);
  revm_command_add(CMD_SHTRM , (void *) cmd_shtrm , (void *) NULL           , 1, HLP_SHTRM);

  /* Trace commands */
  revm_command_add(CMD_TRACES      , (void *) cmd_traces      , (void *) revm_getvarparams , 1, HLP_TRACES);
  revm_command_add(CMD_TRACEADD    , (void *) cmd_traceadd    , (void *) revm_getvarparams , 1, HLP_TRACEADD);
  revm_command_add(CMD_TRACEEXCLUDE, (void *) cmd_traceexclude, (void *) revm_getvarparams , 1, HLP_TRACEEXCLUDE);;
  revm_command_add(CMD_TRACERUN    , (void *) cmd_tracerun    , (void *) revm_getvarparams , 1, HLP_TRACERUN);

  if (world.state.revm_mode == REVM_STATE_TRACER)
    {
      /* Traces aliases */
      revm_command_add(CMD_TRACEADD_CMDLINE , (void *) cmd_traceadd    , (void *) revm_getvarparams, 1, HLP_TRACEADD);
      revm_command_add(CMD_TRACEEXCL_CMDLINE, (void *) cmd_traceexclude, (void *) revm_getvarparams, 1, HLP_TRACEEXCLUDE);
    }
}
