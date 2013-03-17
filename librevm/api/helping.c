/*
* @file librevm/api/helping.c
** @ingroup api
** @brief The help command. 
** Started on  Sat Jan 25 11:19:18 2003 jfv
** $Id$
**
*/
#include "revm.h"

/* The Help command */
int		revm_help(char *command)
{
  revmcmd_t	*cmd;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The general message */
  if (!command)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n The Reverse Engineering Vector Machine (REVM Version %s compiled for %s objects)\n\n\n"
	       " General purpose commands   .::. help, info, cat, exec, edit, sdir, lscripts, profile \n"
	       "                                 quit, exit, load, unload, switch, list, workspace    \n"
	       "                                 setcolor, nocolor, configure, add, sub, mul, div,    \n"
	       "                                 eval						      \n"
	       "                                 mod, cmp, print, stop                                \n"
	       " Ondisk/Memory ELF commands .::. elf, interp, pht, got, sht, rel, notes, dyn, dynsym  \n"
	       "                                 findrel, ctors, write, set, reladd, coreinfo,        \n"
	       "                                 verneed, verdef                                      \n"   
	       " Ondisk only ELF commands   .::. flush, save, sym, stab, append, extend, insert,      \n"
	       "                                 remove, rename, unstrip                              \n"
	       " Function redirect commands .::. redir, flowjack				      \n"
	       " Debugging commands         .::. break, delete, continue, dumpregs, stack, dbgstack   \n"
	       "                                 backtrace, linkmap, step, display, itrace, threads   \n"
	       " Tracing commands           .::. traces, traceadd, tracerun			      \n"
	       " Assembly level commands    .::. disasm, hexa, inspect                                \n"
	       " Code analysis commands     .::. analyse, graph, addgoto, setgvl                      \n"
	       " Types related commands     .::. type, typedef, inform, uninform                      \n"
	       " Transformation commands    .::. rewrite, case, default                               \n"
	       " Reflection commands        .::. vectors, tables, lists, reflect                      \n"
	       " ELF objects flags          .::. fixup, shtrm, sstrip				      \n"
	       " REVM modules commands      .::. modload, modunload, modhelp			      \n"
	       " Network commands           .::. net, netlist, netkill, connect, disconnect, peerslist, rcmd \n"
	       " Available prefixes         .::. all, sort, quiet, verb, alert   \n"
	       " Available Script jumps     .::. jmp, je, jne, jg, jl, jge, jle  \n"
	       " Available modules          .::. modtest, modremap     \n\n"
	       
	       " Type 'help command' for specific information\n\n",
	       
	       REVM_VERSION, 
#if defined(ERESI32)
	       "32 bits"
#elif defined(ERESI64)
	       "64 bits"
#else
	       "Unknown built"
#endif
	       );
      revm_output(logbuf);
    }

  /* Command specific help */
  else
    {
      cmd = hash_get(&cmd_hash, command);
      if (!cmd)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown command", -1);
      revm_output("\t");
      revm_output(command);
      revm_output(" : ");
      revm_output(cmd->help);
      revm_output("\n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

