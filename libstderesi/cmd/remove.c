/**
** @file remove.c
**
** @brief First remove functions for easy scripting
**
** Started on Nov 22 2003 jfv
**
**
** $Id: remove.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/**
 * sect name
 * sym  name 
 */
int		cmd_remove()
{
  elfshsect_t	*symtab;
  char		*name;
  int		err;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Checks (needed because the command takes variable amount of params) */
  if (!world.curjob->curcmd->param[0] || !world.curjob->curcmd->param[1])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  /* Lookup object name */
  name = revm_lookup_string(world.curjob->curcmd->param[1]);

  /* Remove a section */
  if (!strcmp(world.curjob->curcmd->param[0], "sect"))
    err = elfsh_remove_section(world.curjob->current, name);

  /* Remove a symbol after looking up symbol value */
  else if (!strcmp(world.curjob->curcmd->param[0], "sym"))
    {
      symtab = elfsh_get_symtab(world.curjob->current, NULL);
      if (!symtab)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot find symbol table", -1);
      symtab = world.curjob->current->secthash[ELFSH_SECTION_SYMTAB];
      err = elfsh_remove_symbol(symtab, name);
    }

  /* Remove a segment */
  else if (!strcmp(world.curjob->curcmd->param[0], "phdr"))
    err = elfsh_remove_phdr(world.curjob->current, atoi(name));

  /* Error */
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown object type", -1);

  /* Report result */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Object removing %s.\n\n",
	       (err < 0 ? "failed" : "succesfull"));
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
