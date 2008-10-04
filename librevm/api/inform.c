/*
** @file inform.c
** @ingroup librevm_api
** @brief API for annotating program objects.
**
** Started Jan 21 2007 12:57:03 jfv
** $Id: inform.c,v 1.7 2008-02-16 12:32:27 thor Exp $
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
  unsigned int			len;
  static unsigned int		pathsize = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  newexpr = rootexpr = prevexpr = NULL;

  /* We are at the first iteration : create root expression */
  pathsize = snprintf(pathbuf, sizeof(pathbuf), "%s", curpath);
  
#if __DEBUG_EXPRS__
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

#if __DEBUG_EXPRS__
	  fprintf(stderr, " [I] Curchild expr path will be informed recursively (%s) \n", pathbuf);
#endif
	  
	  revm_inform_type_addr(curtype->name, pathbuf, childaddr, newexpr, 0, 0);
	  pathsize += len;
	  revm_inform_subtype(pathbuf, newexpr, curtype, childaddr, translateaddr);
	  pathsize -= len;
	  bzero(pathbuf + pathsize, len);
	}

      /* Simply lookup terminal fields */
      else
	{

#if __DEBUG_EXPRS__
	  fprintf(stderr, " [I] Current TERMINAL path inside loop (%s::%s) \n", 
		  pathbuf, curtype->fieldname);
#endif

	  newexpr->value = revm_object_lookup_real(type, pathbuf, curtype->fieldname, translateaddr);
	  if (!newexpr->value)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Failed to lookup terminal object", NULL);
	  len = snprintf(pathbuf + pathsize, BUFSIZ - pathsize, ".%s", curtype->fieldname);
	  revm_inform_type_addr(curtype->name, pathbuf, childaddr, newexpr, 0, 0);
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
	phdr[index].p_vaddr < addr &&
	addr < phdr[index].p_vaddr + phdr[index].p_memsz)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Add an elemnt to the inform table for a given type, by real address */
revmexpr_t	*revm_inform_type_addr(char		*type, 
				       char		*varname, 
				       eresi_Addr	addr, 
				       revmexpr_t	*expr,
				       unsigned char		print,
				       unsigned char		rec)
{
  revmexpr_t	*ret;
  char		*addrbuf;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, addrbuf, 20, NULL);
  snprintf(addrbuf, 20, "0x%08lX", (unsigned long) addr);
  ret = revm_inform_type(type, varname, addrbuf, expr, print, rec);
  if (ret)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Unable to inform type by address", NULL);
}


/**
 * @brief Add an element to the inform table for a given type 
 */
revmexpr_t	*revm_inform_type(char *type, char *varname, 
				  char *straddr, revmexpr_t *expr,
				  unsigned char print, unsigned char rec)
{
  hash_t	*hash;
  char		buf[BUFSIZ];
  eresi_Addr	addr;
  eresi_Addr	oaddr;
  char		*realname;
  char		*symname;
  revmannot_t	*annot;
  elfsh_SAddr	off;
  aspectype_t	*rtype;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Add element in the good hash table */
  rtype = aspect_type_get_by_name(type);
  snprintf(buf, sizeof(buf), "type_%s", type);
  hash = hash_find(buf);
  if (!hash || !rtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown requested type", NULL);

  /* The address is not precised, to to find it from the varname */
  if (!straddr)
    straddr = varname;

#if __DEBUG_EXPRS__
  fprintf(stderr, "\n [D] Variable %40s LOOKED UP\n", varname);
#endif

  /* The address is given, lookup it */
  if (!hash_get(&exprs_hash, varname))
    realname = revm_lookup_string(varname);
  else
    realname = varname;
  oaddr    = revm_lookup_addr(straddr);

  /* Adding expression and its type to hash tables */
#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Variable %40s TO BE added to exprs_hash with type %s \n", realname, rtype->name);
#endif

  /* Only check for addr range if print flag (manual inform) is on */
  if (print && (!oaddr || !revm_check_addr(world.curjob->curfile, oaddr)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid variable address", NULL);
  if (!realname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid variable name", NULL);

  if (hash_get(&exprs_hash, realname))
    hash_del(&exprs_hash, realname);

  /* If just the address was given, lookup or create a name for the variable */
  if (IS_VADDR(realname))
    {
      sscanf(realname + 2, AFMT, &addr);
      symname = elfsh_reverse_symbol(world.curjob->curfile, addr, &off);
      if (symname && !off)
	realname = strdup(symname);
      else
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, realname, strlen(type) + 20, NULL);
	  snprintf(realname, sizeof(realname), "%s_"AFMT, type, addr);
	}
    }
  addr = oaddr;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, annot, sizeof(revmannot_t), NULL);

  /* Setup current string table offset, type, scope, and representation */
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
	  expr->label  = realname;
	  expr->type   = rtype;

	  // XXX: changed NULL for expr (testing)
	  expr->childs = revm_inform_subtype(realname, NULL, rtype, oaddr, print);
	}
      else if (!rtype->childs)
	expr = revm_simple_expr_create(rtype, realname, NULL);
    }

  /* Else if recursion is asked on an existing expression (set $expr $existingexpr) */
  else if (rec && rtype->childs)
    revm_inform_subtype(realname, expr, rtype, oaddr, print);
  
  /* Register the expression */
  annot->expr = expr;
  hash_set(&exprs_hash, (char *) strdup(realname), (void *) expr);

  /* Adding expression and its type to hash tables */
#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Variable %40s ADDED to exprs_hash with type %s \n", realname, rtype->name);
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
int		revm_uninform_type(char *type, char *varname, unsigned char print)
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

