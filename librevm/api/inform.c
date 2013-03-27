/*
** @file librevm/api/inform.c
** @ingroup api
** @brief API for annotating program objects.
**
** Started Jan 21 2007 12:57:03 jfv
** $Id$
**
*/
#include "revm.h"


/** 
 * @brief Create an expression tree ad-hoc, recursively, and without initialization
 * @param curpath Current path for constructed object
 * @param curexpr Currently constructed expression
 * @param type Type of the root node
 * @param addr Address of currently constructed object
 * @param translateaddr Translate pointers between analyzed and analyzer address space
 * @return 
 */
static revmexpr_t	*revm_inform_subtype(char		*curpath,
					     revmexpr_t		*curexpr,
					     aspectype_t	*type,
					     eresi_Addr		addr,
					     char		translateaddr)
{
  char			pathbuf[BUFSIZ];
  aspectype_t		*curtype;
  revmexpr_t		*newexpr, *rootexpr, *prevexpr;
  eresi_Addr		childaddr;
  u_int			len;
  static u_int		pathsize = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  newexpr = rootexpr = prevexpr = NULL;

  /* We are at the first iteration : create root expression */
  pathsize = snprintf(pathbuf, sizeof(pathbuf), "%s", curpath);
  
#if __DEBUG_INFORM__
  fprintf(stderr, " [I] Current expr path before loop (%s) \n", pathbuf);
#endif

  /* Iterate over all fields */
  for (curtype = type->childs; curtype; curtype = curtype->next)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, newexpr, sizeof(revmexpr_t), NULL);
      newexpr->type   = curtype;
      childaddr       = addr + curtype->off;
      newexpr->label  = curtype->fieldname;
      newexpr->parent = curexpr;

      /* Recurse over record children. */
      /* Do not recurse on pointers else we get infinite loops ! */
      if (curtype->childs && !curtype->isptr)
	{
	  len = snprintf(pathbuf + pathsize, BUFSIZ - pathsize, ".%s", curtype->fieldname);

#if __DEBUG_INFORM__
	  fprintf(stderr, " [I] Curchild expr path will be informed recursively (%s) \n", pathbuf);
#endif
	  
	  revm_inform_type(curtype->name, pathbuf, childaddr, newexpr, 0, 0);
	  pathsize += len;
	  revm_inform_subtype(pathbuf, newexpr, curtype, childaddr, translateaddr);
	  pathsize -= len;
	  bzero(pathbuf + pathsize, len);
	}

      /* Simply lookup terminal fields */
      else
	{

#if __DEBUG_INFORM__
	  fprintf(stderr, " [I] Current TERMINAL path inside loop (%s::%s) \n", 
		  pathbuf, curtype->fieldname);
#endif

	  newexpr->value = revm_object_lookup_real(type, pathbuf, curtype->fieldname, translateaddr);
	  if (!newexpr->value)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Failed to lookup terminal object", NULL);

	  len = snprintf(pathbuf + pathsize, BUFSIZ - pathsize, ".%s", curtype->fieldname);
	  revm_inform_type(curtype->name, pathbuf, childaddr, newexpr, 0, 0);
	  bzero(pathbuf + pathsize, len);
	}
      
      /* Link fields and children in the constructed expression */
      if (curexpr)
	{
	  rootexpr = curexpr;
	  if (prevexpr)
	    {
	      prevexpr->next = newexpr;
	      prevexpr = newexpr;
	    }
	  else
	    {
	      curexpr->childs = newexpr;
	      prevexpr = newexpr;
	    }
	}
      else
	{
	  if (prevexpr)
	    {
	      prevexpr->next = newexpr;
	      prevexpr = newexpr;
	    }
	  else
	    rootexpr = prevexpr = newexpr;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rootexpr);
}



/* Print registered variables for a given type */
int		revm_informed_print(char *name)
{
  hash_t	*hash;
  char		buf[BUFSIZ];
  revmannot_t	*annot;
  char		**keys;
  int		index;
  int		keynbr;

  /* Preliminary checks */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  snprintf(buf, sizeof(buf), "type_%s", name);
  hash = hash_find(buf);
  if (!hash)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown informed type", -1);
  
  /* Retreive hash table content and print it */
  keys = hash_get_keys(hash, &keynbr);  
  if (keynbr)
    revm_output("  .:: Registered variables for this type \n\n");
  for (index = 0; index < keynbr; index++)
    {
      annot = hash_get(hash, keys[index]);
      snprintf(buf, sizeof(buf), "  + ["AFMT"] %-30s \n", annot->addr, keys[index]);
      revm_output(buf);
    }

  /* End message */
  if (!keynbr)
    revm_output("  .:: No registered variables for this type \n\n");
  else
    {
      snprintf(buf, sizeof(buf), 
	       "\n .:: Displayed %u registered variables for type %s \n\n",
	       keynbr, name);
      revm_output(buf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Check the validity of an address 
 */
int		revm_check_addr(elfshobj_t *obj, eresi_Addr addr)
{
  elfsh_Phdr	*phdr;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (index = 0, phdr = obj->pht; index < obj->hdr->e_phnum; index++)
    if (phdr[index].p_type == PT_LOAD && 
	phdr[index].p_vaddr <= addr &&
	addr < phdr[index].p_vaddr + phdr[index].p_memsz)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Add an element to the inform table for a given type, by real address */
revmexpr_t	*revm_inform_type_addr(char		*type, 
				       char		*varname, 
				       eresi_Addr	addr, 
				       revmexpr_t	*expr,
				       u_char		print,
				       u_char		rec)
{
  revmexpr_t	*ret;
  char		*addrbuf;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, addrbuf, 20, NULL);
  snprintf(addrbuf, 20, XFMT, (eresi_Addr) addr);
  ret = revm_inform_toplevel(type, varname, addrbuf, expr, print, rec);
  if (ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Unable to inform type by address", NULL);
}


/**
 * @brief Add an element to the inform table for a given type 
 */
revmexpr_t	*revm_inform_toplevel(char *type, char *varname, 
				      char *straddr, revmexpr_t *expr,
				      u_char print, u_char rec)
{
  eresi_Addr	oaddr;
  char		*realname;
  char		*symname;
  elfsh_SAddr	off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* The address is not precised, to to find it from the varname */
  if (!straddr)
    straddr = varname;

#if __DEBUG_INFORM__
  printf("\n [D] Variable %40s TOPLEVEL INFORM ENTER \n", varname);
#endif

  /* The address is given, look it up */
  if (!revm_expr_get(varname))
    realname = revm_lookup_string(varname);
  else
    realname = varname;
  oaddr    = revm_lookup_addr(straddr);

  /* Adding expression and its type to hash tables */
#if __DEBUG_INFORM__
  printf(" [D] Variable %40s TO BE added to local exprs table with type %s \n", 
	  realname, type);
#endif

  /* Only check for addr range if print flag (manual inform) is on */
  if (print && !revm_check_addr(world.curjob->curfile, oaddr))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid variable address", NULL);
  if (!realname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid variable name", NULL);

  /* If just the address was given, lookup or create a name for the variable */
  if (IS_VADDR(realname))
    {
      sscanf(realname + 2, AFMT, &oaddr);
      symname = elfsh_reverse_symbol(world.curjob->curfile, oaddr, &off);
      if (symname && !off)
	realname = strdup(symname);
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, realname, strlen(type) + 20, NULL);
	  snprintf(realname, sizeof(realname), "%s_"AFMT, type, oaddr);
	}
    }

  /* It can happens that the realname lacks a variable prefix, in that case add it now */
  if (*realname != REVM_VAR_PREFIX)
    {
      varname = alloca(strlen(realname) + 2);
      *varname = REVM_VAR_PREFIX;
      strcpy(varname + 1, realname);
      realname = varname;
    }

  /* Now do the real job */
  expr = revm_inform_type(type, realname, oaddr, expr, print, rec);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to inform type", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}


/** Add type information for a memory address */
revmexpr_t	*revm_inform_type(char *type, char *realname, 
				  eresi_Addr addr, revmexpr_t *expr,
				  u_char print, u_char rec)
{
  hash_t	*hash;
  char		buf[BUFSIZ];
  revmannot_t	*annot;
  aspectype_t	*rtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Add element in the good hash table */
  rtype = aspect_type_get_by_name(type);
  snprintf(buf, sizeof(buf), "type_%s", type);
  hash = hash_find(buf);
  if (!hash || !rtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown requested type", NULL);

  /* Setup current string table offset, type, scope, and representation */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, annot, sizeof(revmannot_t), NULL);
  annot->nameoff = revm_strtable_add(realname);
  annot->typenum = rtype->type; 
  annot->scope = EDFMT_SCOPE_GLOBAL; 
  annot->addr = addr;

  /* This must be done before informing subtypes, as we will need 
     the annotation to create subfields of pointed expressions (e.g. hash/list elems) */
  hash_set(hash, strdup(realname), (void *) annot);
  
  /* If no expression was given (manual inform) we need to create one ad-hoc */
  if (!expr)
    {
      
      /* FIXME: Note that expr can remain NULL if rec == 0 and childs != NULL (arrays) */
      if (rtype->childs && rec)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);
	  expr->strval = NULL;
	  expr->label  = strdup(realname);
	  expr->type   = rtype;
	  expr->childs = revm_inform_subtype(realname, NULL, rtype, addr, print);
	}
      else if (!rtype->childs)
	expr = revm_simple_expr_create(rtype, realname, NULL);
    }

  /* Else if recursion is asked on an existing expression (set $expr $existingexpr) */
  else if (rec && rtype->childs)
    revm_inform_subtype(realname, expr, rtype, addr, print);
  
  /* Register the expression */
  annot->expr = expr;
  if (expr)
    expr->annot = annot;

  hash_set(&world.curjob->recur[world.curjob->curscope].exprs, 
	   (char *) strdup(realname), (void *) expr);

  /* Adding expression and its type to hash tables */
#if __DEBUG_INFORM__
  printf(" [D] Variable %40s ADDED to local exprs hash (scope %u) with type %s \n", 
	 realname, world.curjob->curscope, rtype->name);
#endif

  /* Success message and exit */
  if (print)
    {
      snprintf(buf, sizeof(buf), 
	       " [*] Type %s succesfully informed of variable %s [" XFMT "]\n\n", 
	       type, realname, annot->addr);
      revm_output(buf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}



/* Add an element to the inform table for a given type */
int		revm_uninform_type(char *type, char *varname, u_char print)
{
  hash_t	*hash;
  char		buf[BUFSIZ];
  char		*realname;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
   
  /* Grab element in the good hash table */
  snprintf(buf, sizeof(buf), "type_%s", type);
  hash = hash_find(buf);
  if (!hash)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown requested type", -1);

  /* In case we have to remove everything at once */
  if (!varname)
    {
      hash_empty(buf);
      if (print)
	{
	  snprintf(buf, sizeof(buf), 
		   " [*] Hash information for type %s succesfully reset \n\n", type);
	  revm_output(buf);
	}
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* In case we just have to remove one variable */
  realname = revm_lookup_string(varname);
  if (!realname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid variable name", -1);

  if (!hash_get(hash, realname))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown typed variable", -1);

  if (print)
    {
      snprintf(buf, sizeof(buf), 
	       " [*] Type %s succesfully uninformed of object %s \n\n", 
	       type, realname);
      revm_output(buf);
    }
  hash_del(hash, realname);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

