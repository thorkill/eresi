/*
** help.c for elfsh
** 
** Started on  Sat Jan 25 11:19:18 2003 mayhem
** Updated on  Mon Aug 15 23:59:18 2005 mayhem
*/
#include "elfsh.h"



/* The ELFsh modules Help command */
int		cmd_modhelp()
{
  elfshmod_t	*mod;
  u_int		id;
  char		buf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  mod = 0;
  id = atoi(world.curjob->curcmd->param[0]);
  if (!id)
    {
      if (access(world.curjob->curcmd->param[0], R_OK) != 0)
	{
	  snprintf(buf, sizeof(buf), "%s%s", ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	  if (access(buf, R_OK) != 0)
	    {
	      snprintf(buf, sizeof(buf), "%s%s.so",
		       ELFSH_MODPATH, world.curjob->curcmd->param[0]);
	      if (access(buf, R_OK) != 0)
		ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
				  "Cannot find module", -1);
	    }
	  mod = hash_get(&mod_hash, buf);
	}
    }
  else
    mod = vm_getmod(id);
  
  if (mod == NULL || mod->help == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Module unavailable", -1);
  mod->help();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* The Help command */
int		cmd_help()
{
  elfshcmd_t	*cmd;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The general message */
  if (!world.curjob->curcmd->param[0])
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n\t\t The ELF shell %s (compiled for %s objects) \n\n"
	       " General purpose commands   .::. help, info, cat, exec, edit, sdir, lscripts, profile \n"
	       "                                 quit, exit, load, unload, switch, list, workspace    \n"
	       "                                 setcolor, nocolor				      \n"
	       " Ondisk/Memory ELF commands .::. elf, interp, pht, got, sht, rel, notes, dyn, dynsym \n"
	       "                                 findrel, ctors, disasm, hexa, set, get, write, print \n"
	       "                                 add, sub, mul, div, mod, cmp, reladd, redir \n"
	       " Debugger commands          .::. break, delete, continue, dumpregs, stack, dbgstack \n"
	       "                                 backtrace, linkmap, step \n"
	       " ELFsh modules commands     .::. modload, modunload, modhelp \n"
	       " ELF objects flags          .::. fixup, shtrm, sstrip \n"                        
	       " Ondisk only ELF commands   .::. flush, save, sym, stab, append, extend, insert, remove \n"
	       " Network commands           .::. net, netlist, netkill, connect, disconnect, peerslist, rcmd \n\n"
	       " Available prefixes         .::. all, sort, quiet, verb, alert \n"
	       " Available Script jumps     .::. jmp, je, jne, jg, jl, jge, jle \n"
	       " Available modules          .::. modtest, modremap, modflow \n\n"
	       
	       " Type 'help command' for specific information\n\n",
	       
	   ELFSH_VERSION, 
#if defined(ELFSH32)
	       "32 bits"
#elif defined(ELFSH64)
	       "64 bits"
#else
	       "Unknown built"
#endif
	       );
      vm_output(logbuf);
    }

  /* Command specific help */
  else
    {
      cmd = hash_get(&cmd_hash, world.curjob->curcmd->param[0]);
      if (!cmd)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown command", -1);
      vm_output("\t");
      vm_output(world.curjob->curcmd->param[0]);
      vm_output(" : ");
      vm_output(cmd->help);
      vm_output("\n\n");
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

