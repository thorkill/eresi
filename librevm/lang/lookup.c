/*
** lookup.c for librevm in ERESI
**
** Various object lookup functions built in the language
**
** Started      Nov 21 2003 mayhem
** Last update  Mar 01 2007 mayhem
**
**
** $Id: lookup.c,v 1.12 2007-03-07 16:45:36 thor Exp $
**
*/
#include "revm.h"


/* Support for double (or multiple) variables : $$name, $$$name, etc */
revmobj_t		*vm_lookup_var(char *param)
{
  revmobj_t		*ptr;
  int			indir;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);

  for (indir = 0; *param == ELFSH_VARPREF; indir++, param++);  
  ptr = NULL;
  if (!*param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  while (indir--)
    {
      ptr = (void *) hash_get(&vars_hash, param);
      if (ptr == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown variable", (NULL));
      if (indir)
	{
	  if (vm_convert_object(ptr, ASPECT_TYPE_STR) < 0 || !ptr->immed)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Unknown variable", (NULL));
	  param = ptr->immed_val.str;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
}


/* Get address value */
elfsh_Addr		vm_lookup_addr(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  revmobj_t		*ptr;
  elfsh_Addr	       	val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", 0);

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(world.curjob->current, param);
  if (sym != NULL && sym->st_value > 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(world.curjob->current, param);
  if (sym != NULL && sym->st_value > 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);

  /* Lookup a variable */
  ptr = vm_lookup_var(param);
  if (ptr)
    {
      if (ptr->type == ASPECT_TYPE_LONG  ||
	  ptr->type == ASPECT_TYPE_CADDR ||
	  ptr->type == ASPECT_TYPE_DADDR)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (ptr->immed ? ptr->immed_val.ent : 
		       (elfsh_Addr) ptr->get_obj(ptr->parent)));
    }

  /* Lookup a constant */
  /* FIXME : Constants must be differentiated by their size ! */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, actual->val);
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, val);

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup address object", 0);
}



/* Get immediate value */
revmobj_t		*vm_lookup_immed(char *param)
{
  elfsh_Sym		*sym;
  revmconst_t		*actual;
  char			eol;
  int			ret;
  char			lbuf[4096];
  revmobj_t		*ptr;
  elfsh_Addr	       	val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", (NULL));

  /* Lookup a known variable */
  ptr = vm_lookup_var(param);
  if (ptr)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);

  /* Support for lazy creation of variables */
  if (*param == ELFSH_VARPREF)
    {
      ptr = vm_create_IMMED(ASPECT_TYPE_UNKNOW, 1, 0);
      hash_add(&vars_hash, strdup(++param), ptr);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
    }

  /* Not necessary to call those functions if there is no loaded file */
  if (world.curjob->current)
    {

      /* Lookup .symtab */
      sym = elfsh_get_symbol_by_name(world.curjob->current, param);
      if (sym != NULL)
	{
	  ptr = vm_create_LONG(0, sym->st_value);
	  goto good;
	}
      
      /* Lookup .dynsym */
      sym = elfsh_get_dynsymbol_by_name(world.curjob->current, param);
      if (sym != NULL)
	{
	  ptr = vm_create_LONG(0, sym->st_value);
	  goto good;
	}
    }


  /* FIXME: Constants must be differentiated by their size ! */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    {
      ptr = vm_create_IMMED(ASPECT_TYPE_INT, 0, actual->val);
      goto good;
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = vm_create_LONG(0, val);
      goto good;
    }

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ptr = vm_create_LONG(0, val);
      goto good;
    }
  
  /* Lookup a supplied string */
  bzero(lbuf, sizeof(lbuf));
  ret = sscanf(param, "%[^\n]4095%c", lbuf, &eol);
  if (ret == 1)
    {
      ptr = vm_create_IMMEDSTR(0, strdup(lbuf));
 
      goto good;
    }

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup object", (NULL));
  
  /* Match */
 good:
  
  /* Now replace \x00 patterns if any */
  if (ptr->type == ASPECT_TYPE_STR)
    vm_filter_zero(ptr->immed_val.str);

  /* We matched -- returns OK */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr);
}



/* Lookup an index */
u_int     		vm_lookup_index(char *param)
{
  revmconst_t		*actual;
  revmobj_t		*ptr;
  char			eol;
  int			ret;
  u_int		       	val;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", 0);

  /* Support for lazy creation of variables */
  ptr = vm_lookup_var(param);
  if (ptr)
    {
      if (ptr->type == ASPECT_TYPE_INT   || 
	  ptr->type == ASPECT_TYPE_SHORT || 
	  ptr->type == ASPECT_TYPE_BYTE  || 
	  ptr->type == ASPECT_TYPE_LONG  ||
	  ptr->type == ASPECT_TYPE_CADDR ||
	  ptr->type == ASPECT_TYPE_DADDR)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		      (ptr->immed ? (u_int) ptr->immed_val.ent : 
		       (u_int) ptr->get_obj(ptr->parent)));
      
      if (vm_convert_object(ptr, ASPECT_TYPE_INT) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid parameter", 0);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    (ptr->immed ? (u_int) ptr->immed_val.ent : 
		     (u_int) ptr->get_obj(ptr->parent)));
    }

  /* Lookup a constant */
  actual = hash_get(&const_hash, param);
  if (actual != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (u_int) actual->val);
  
  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* Lookup decimal numeric value */
  ret = sscanf(param, UFMT "%c", &val, &eol);
  if (ret == 1)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (val));

  /* We do not match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup valid object", ((u_int) -1));
}



/* Lookup an index */
char			*vm_lookup_string(char *param)
{
  revmobj_t		*ptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL parameter", (NULL));

  /* Support for lazy creation of variables */
  ptr = vm_lookup_var(param);
  if (!ptr)
    {
      vm_filter_zero(param);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, param);
    }
  if (ptr->type != ASPECT_TYPE_STR)
    {
      vm_convert_object(ptr, ASPECT_TYPE_STR);
      if (ptr->type != ASPECT_TYPE_STR)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unexpected object type", NULL);
    }
  if (ptr->get_name)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		  ptr->get_name(ptr->root, ptr->parent));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		(ptr->immed ? ptr->immed_val.str : 
		 (char *) ptr->get_obj(ptr->parent)));
}



/* Lookup the file pointed by name */
elfshobj_t		*vm_lookup_file(char *param)
{
  u_int			idx;
  revmobj_t		*ptr;
  elfshobj_t		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Support for variable file lookup */
  idx = 0;
  if (!param)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL parameter", (NULL));
  ptr = vm_lookup_var(param);
  if (!ptr)
    idx = atoi(param);
  else if (ptr->type == ASPECT_TYPE_INT)
    idx = ptr->immed_val.ent;
  else if (ptr->type == ASPECT_TYPE_STR)
    param = ptr->immed_val.str;
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unexpected variable type", (NULL));

  /* Let's ask the hash table for the current working file */
  ret = (idx ? vm_getfile(idx) : hash_get(&file_hash, param));
  if (ret == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "File not found", (NULL));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
