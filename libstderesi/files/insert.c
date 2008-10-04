/*
** @file insert.c
** @ingroup libstderesi
** @brief First insert functions for easy scripting.
**
** Started     Nov 22 01:26:01 2003 jfv
**
**
** $Id: insert.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"



/**
 * sect name maptype size align
 * sym  name value size 
 * phdr type value size 
 */
int		cmd_insert()
{
  elfshsect_t	*sct;
  revmexpr_t	*expr;
  revmobj_t	*obj;
  elfsh_Sym	sym;
  elfsh_Phdr	phdr;
  unsigned int		size;
  char		*name;
  char		type;
  elfsh_Word	ptype;
  eresi_Addr	val;
  unsigned int		modulo;
  char		*param0, *param1, *param2, *param3, *param4;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  param0 = world.curjob->curcmd->param[0];
  param1 = world.curjob->curcmd->param[1];
  param2 = world.curjob->curcmd->param[2];
  param3 = world.curjob->curcmd->param[3];
  param4 = world.curjob->curcmd->param[4];

  /* Checks (needed because the command takes variable amount of params) */
  if (!param0 || !param1 || !param2)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  /* Lookup 2nd parameter depending on first parameter */
  if (!strcmp(param0, "phdr") || !strcmp(param0, "rphdr"))
    {
      expr = revm_lookup_param(param1);
      if (!expr || !expr->value || !expr->value->otype ||
	  (expr->value->otype->type != ASPECT_TYPE_LONG && 
	   expr->value->otype->type != ASPECT_TYPE_INT))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid phdr type", -1);
      obj = expr->value;
      ptype = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      name = NULL;
    }
  else
    {
      expr = revm_lookup_param(param1);
      if (!expr || !expr->value || !expr->value->otype || expr->value->otype->type != ASPECT_TYPE_STR)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid object name", -1);
      obj = expr->value;
      name = (obj->immed ? obj->immed_val.str : obj->get_name(obj->root, obj->parent));
      ptype = 0;
    }

  /* Lookup object size (4rth optional parameter) */
  if (param3)
    {
      expr = revm_lookup_param(param3);
      if (!expr || !expr->value || !expr->value->otype || 
	  (expr->value->otype->type != ASPECT_TYPE_INT && expr->value->otype->type != ASPECT_TYPE_LONG))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid object size", -1);
      obj = expr->value;
      size = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
    }
  else
    size = elfsh_get_pagesize(world.curjob->curfile); 

  /* Lookup object alignment (5th optional parameter) */
  if (param4)
    modulo = atoi(param4);
  else
    modulo = 0;

  /* Insert a section after looking up injection type */
  /* Third parameter indicate injection type */
  if (!strcmp(param0, "sect"))
    {
      type = (!strcmp(param2, "code")  ? ELFSH_CODE_INJECTION :
	      !strcmp(param2, "data")  ? ELFSH_DATA_INJECTION :
	      !strcmp(param2, "unmap") ?
	      ELFSH_UNMAPPED_INJECTION : ELFSH_UNKNOWN_INJECTION);
      if (type == ELFSH_UNKNOWN_INJECTION)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown injection type", -1);
      sct = elfsh_insert_section(world.curjob->curfile, name, 
				 NULL, type, size, modulo);
      if (!sct)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Unable to insert new section", -1);
    }
  
  /* Insert a symbol after looking up symbol value */
  /* Third parameter indicate existing symbol name or value */
  else if (!strcmp(param0, "sym"))
    {
      expr = revm_lookup_param(param2);
      if (!expr || !expr->value || !expr->value->otype || 
	  (expr->value->otype->type != ASPECT_TYPE_LONG && expr->value->otype->type != ASPECT_TYPE_INT))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid symbol value", -1);
      obj = expr->value;
      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
      if (!elfsh_get_symtab(world.curjob->curfile, NULL))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot retreive symbol table", -1);

      sym = elfsh_create_symbol(val, size, 0, 0, 0, 0);
      if (elfsh_insert_symbol(world.curjob->curfile->secthash[ELFSH_SECTION_SYMTAB],
			      &sym, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Cannot insert symbol", -1);


    }

  /* Insert PHT entries */
  /* Third parameter indicate symbol name or value */
  else if (!strcmp(param0, "phdr") ||
	   !strcmp(param0, "rphdr"))
    {
      expr = revm_lookup_param(param2);
      if (!expr || !expr->value || !expr->value->otype || 
	  (expr->value->otype->type != ASPECT_TYPE_LONG && expr->value->otype->type != ASPECT_TYPE_INT))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Invalid PHDR base address", -1);
      obj = expr->value;
      val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));

      // type, addr, size, alignment 
      phdr = elfsh_create_phdr(ptype, val, size, 0);
      if (!strcmp(param0, "phdr"))
	{
	  if (NULL == elfsh_insert_phdr(world.curjob->curfile, &phdr))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to insert new PHDR", -1);
	}
      else if (NULL == elfsh_insert_runtime_phdr(world.curjob->curfile, &phdr))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to insert new runtime PHDR", -1);
    }
  
  /* Error */
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unknown object type", -1);

  /* Report result */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Object insertion succesfull\n\n");
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
