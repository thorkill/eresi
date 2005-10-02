/*
** remove.c for elfsh
**
** First remove functions for easy scripting
**
** Started on Nov 22 2003 mayhem
**
*/
#include "elfsh.h"



/* sect name */
/* sym  name */
int		cmd_remove()
{
  elfshsect_t	*symtab;
  char		*name;
  int		err;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Checks (needed because the command takes variable amount of params) */
  if (!world.curjob->curcmd->param[0] || !world.curjob->curcmd->param[1])
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  /* Lookup object name */
  name = vm_lookup_string(world.curjob->curcmd->param[1]);

  /* Remove a section */
  if (!strcmp(world.curjob->curcmd->param[0], "sect"))
    err = elfsh_remove_section(world.curjob->current, name);

  /* Remove a symbol after looking up symbol value */
  else if (!strcmp(world.curjob->curcmd->param[0], "sym"))
    {
      symtab = elfsh_get_symtab(world.curjob->current, NULL);
      if (!symtab)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot find symbol table", -1);
      symtab = world.curjob->current->secthash[ELFSH_SECTION_SYMTAB];
      err = elfsh_remove_symbol(symtab, name);
    }

  /* Remove a segment */
  else if (!strcmp(world.curjob->curcmd->param[0], "phdr"))
    err = elfsh_remove_phdr(world.curjob->current, atoi(name));

  /* Error */
  else
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown object type", -1);

  /* Report result */
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Object removing %s.\n\n",
	       (err < 0 ? "failed" : "succesfull"));
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
