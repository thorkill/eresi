/*
** insert.c for elfsh
**
** First insert functions for easy scripting
**
** Started     Nov 22 01:26:01 2003 mayhem
**
*/
#include "elfsh.h"



/* sect name maptype size align */
/* sym  name value size */
/* phdr type value size */
int		cmd_insert()
{
  elfshsect_t	*sct;
  elfshpath_t	*obj;
  elfsh_Sym	sym;
  elfsh_Phdr	phdr;
  u_int		size;
  char		*name;
  char		type;
  int		err;
  elfsh_Word	ptype;
  elfsh_Addr	val;
  u_int		modulo;
  char		logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Checks (needed because the command takes variable amount of params) */
  if (!world.curjob->curcmd->param[0] || !world.curjob->curcmd->param[1] || 
      !world.curjob->curcmd->param[2])
    ELFSH_SETERROR("[elfsh:cmd_insert] Invalid parameters.\n", -1);
  err = 0;

  /* Lookup 2nd parameter depending on first parameter */
  if (strcmp(world.curjob->curcmd->param[0], "phdr") ||
      strcmp(world.curjob->curcmd->param[0], "rphdr"))
    {
      obj = vm_lookup_param(world.curjob->curcmd->param[1], 1);
      if (!obj || obj->type != ELFSH_OBJSTR)
	ELFSH_SETERROR("[elfsh:cmd_insert] Invalid object name.\n", -1);
      name = (obj->immed ? obj->immed_val.str : 
	      obj->get_name(obj->root, obj->parent));
      ptype = 0;
    }
  else
    {
      obj = vm_lookup_param(world.curjob->curcmd->param[1], 1);
      if (!obj || (obj->type != ELFSH_OBJLONG && obj->type != ELFSH_OBJINT))
	ELFSH_SETERROR("[elfsh:cmd_insert] Invalid phdr type.\n", -1);
      ptype = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      name = NULL;
    }

  /* Lookup object size (4rth optional parameter) */
  if (world.curjob->curcmd->param[3])
    {
      obj = vm_lookup_param(world.curjob->curcmd->param[3], 1);
      if (!obj || (obj->type != ELFSH_OBJINT && obj->type != ELFSH_OBJLONG))
	ELFSH_SETERROR("[elfsh:cmd_insert] Invalid object size.\n", -1);
      size = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
    }
  else
    size = elfsh_get_pagesize(world.curjob->current); 

  /* Lookup object alignment (5th optional parameter) */
  if (world.curjob->curcmd->param[4])
    modulo = atoi(world.curjob->curcmd->param[4]);
  else
    modulo = 0;

  /* Insert a section after looking up injection type */
  /* Third parameter indicate injection type */
  if (!strcmp(world.curjob->curcmd->param[0], "sect"))
    {
      type = (!strcmp(world.curjob->curcmd->param[2], "code")  ? ELFSH_CODE_INJECTION :
	      !strcmp(world.curjob->curcmd->param[2], "data")  ? ELFSH_DATA_INJECTION :
	      !strcmp(world.curjob->curcmd->param[2], "unmap") ?
	      ELFSH_UNMAPPED_INJECTION : ELFSH_UNKNOWN_INJECTION);
      if (type == ELFSH_UNKNOWN_INJECTION)
	ELFSH_SETERROR("[elfsh:cmd_insert] Unknown injection type.\n", -1);
      sct = elfsh_insert_section(world.curjob->current, name, 
				 NULL, type, size, modulo);
      if (!sct)
	err = -1;
    }
  
  /* Insert a symbol after looking up symbol value */
  /* Third parameter indicate existing symbol name or value */
  else if (!strcmp(world.curjob->curcmd->param[0], "sym"))
    {
      obj = vm_lookup_param(world.curjob->curcmd->param[2], 1);
      if (!obj || (obj->type != ELFSH_OBJLONG && obj->type != ELFSH_OBJINT))
	ELFSH_SETERROR("[elfsh:cmd_insert] Invalid symbol value.\n", -1);

      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      if (!elfsh_get_symtab(world.curjob->current, NULL))
	ELFSH_SETERROR("[elfsh:cmd_insert] Cannot retreive symbol table.\n",
		       -1);

      sym = elfsh_create_symbol(val, size, 0, 0, 0, 0);
      err = elfsh_insert_symbol(world.curjob->current->secthash[ELFSH_SECTION_SYMTAB],
				&sym, name);
    }

  /* Insert PHT entries */
  /* Third parameter indicate symbol name or value */
  else if (!strcmp(world.curjob->curcmd->param[0], "phdr") ||
	   !strcmp(world.curjob->curcmd->param[0], "rphdr"))
    {

      obj = vm_lookup_param(world.curjob->curcmd->param[2], 1);
      if (!obj || (obj->type != ELFSH_OBJLONG && obj->type != ELFSH_OBJINT))
	ELFSH_SETERROR("[elfsh:cmd_insert] Invalid PHDR base address.\n", -1);
      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));

      // type, addr, size, alignment 
      phdr = elfsh_create_phdr(ptype, val, size, 0);
      if (!strcmp(world.curjob->curcmd->param[0], "phdr"))
	{
	  if (NULL == elfsh_insert_phdr(world.curjob->current, &phdr))
	    err = -1;
	}
      else if (NULL == elfsh_insert_runtime_phdr(world.curjob->current, &phdr))
	err = -1;
    }
  
  /* Error */
  else
    ELFSH_SETERROR("[elfsh:cmd_insert] Unknown object type.\n", -1);

  /* Report result */
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Object insertion %s.\n\n",
	       (err < 0 ? "failed" : "succesfull"));
      vm_output(logbuf);
    }
  
  return (err);
}
