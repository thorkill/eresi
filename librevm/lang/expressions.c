/*
** expressions.c for librevm in ERESI
**
** Implementation of scripting declarations for meta-language variables
**
** Started on Jun 23 2007 23:39:51 mayhem
** $Id: expressions.c,v 1.4 2007-07-12 17:43:27 may Exp $
*/
#include "revm.h"



/* Get the real value of the parameter string for further revmobj initialization */
static revmexpr_t *revm_expr_read(char **datavalue)
{
  revmexpr_t	*expr;
  char		*datastr;
  u_int		opening;
  u_int		closing;
  u_int		beginning;
  char		*namend;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  datastr = *datavalue;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr,
	 sizeof(revmexpr_t), NULL);
  
  /* First get the field (or top-level type) name */
  expr->label = datastr;
  while (*datastr && *datastr != '(' && *datastr != ':')
    datastr++;
  if (!*datastr)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to get root field or type name", NULL);
    }

  /* Implicit field/type name */
  else if (expr->label == datastr)
    expr->label = NULL;

  /* Eat as many '(' or a ':' */
  namend = datastr;
  for (opening = 0; *datastr == '('; datastr++)
    beginning = ++opening;
  if (!opening && *datastr == ':')
    *datastr++ = 0x00;
  if (!*datastr)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid field syntax", NULL);
    }
  
  /* Get the nesting level */
  expr->strval = datastr;
  for (closing = 0; *datastr; datastr++)
    switch (*datastr)
      {
      case '(':
	opening++;
	break;
      case ')':
	closing++;
	if (closing > opening)
	  {
	    XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid closing brace in field value", NULL);
	  }
	break;
      case ',':
	if (opening == closing)
	  {
	    *datavalue = datastr + 1; 
	    for (*datastr-- = 0x00; *datastr == ')' && beginning-- > 0; 
		 *datastr-- = 0x00)
	      *namend++ = 0x00;

#if __DEBUG_EXPRS__
	    fprintf(stderr, " [D] NEW REVMEXPR = %s :: %s \n", 
		    expr->label, expr->strval);
#endif

	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
	  }
	break;
      default:
	break;
      }

  /* Well-bracketed end of string : OK */
  if (opening == closing)
    {
      for (*datavalue = datastr--; *datastr == ')' && beginning-- > 0; 
	   *datastr-- = 0x00)
	*namend++ = 0x00;
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] NEW REVMEXPR =  %s ::: %s \n", 
	      expr->label, expr->strval);
#endif

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
    }
  
  /* Default error */
  XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Malformed parent field value", NULL);
}


/* Initialize a (potentially record) ERESI variable */
static revmexpr_t	*revm_expr_init(char		*curpath, 
					revmexpr_t	*curexpr,
					aspectype_t	*curtype, 
					void		*srcdata,
					char		*datavalue)
{
  static u_int	toplevel = 1;
  static u_int	pathsize = 0;
  char		pathbuf[BUFSIZ + 1] = {0x00};
  char		*recpath;
  revmexpr_t	*newexpr, *rootexpr, *next, *prevexpr;
  void		*childata;
  aspectype_t	*childtype;
  revmobj_t	*curdata;
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!curexpr)
    toplevel = 1;
  newexpr = next = rootexpr = prevexpr = NULL;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Entering revm_expr_init with toplevel = %u\n", toplevel);
#endif

  /* Preliminary processing */
  if (*curpath == '$')
    {
      strncpy(pathbuf, curpath, BUFSIZ);
      recpath = pathbuf;
      pathsize = strlen(curpath);
    }
  else
    recpath = curpath;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Current expr path(%s) \n", recpath);
#endif

  /* Construct the expression until end of ascii string */
  while (*datavalue)
    {
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] Current datavalue = (%s) \n", datavalue);
#endif

      /* Read next typed expression in string */
      newexpr    = revm_expr_read(&datavalue);

      /* If we are at the root expression, type might have been implicit */
      if (toplevel && newexpr->label)
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] Top level type with explicit type label = (%s) strval = (%s) \n", 
		  newexpr->label, newexpr->strval);
#endif

	  if (strcmp(newexpr->label, curtype->name))
	    {
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Type request / specification incompatible", NULL);	  
	    }
	  datavalue = newexpr->strval;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	  toplevel = 0;
	  continue;
	}
      else if (!curexpr && !newexpr->label)
	{
	  toplevel = 0;
	  datavalue = newexpr->strval;

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] No top-level label, continuing with datavalue = %s \n", datavalue);
#endif

	  continue;
	}
      else
	toplevel = 0;


      /* Else we check if the field has children */
      childtype = aspect_type_get_child(curtype, newexpr->label);
      if (!childtype)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	  pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid child structure for variable", NULL);
	}
      newexpr->typeid = childtype->type;

      /* Duplicate names cause they are on the stack now */
      newexpr->label = (char *) aproxy_strdup(newexpr->label);
      newexpr->strval = (char *) aproxy_strdup(newexpr->strval);

      /* Non-terminal case : we will need to recurse */
      if (childtype->childs)
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] NOW RECORD field, recursing ! \n");
#endif

	  childata = (char *) srcdata + childtype->off;
	  len = snprintf(pathbuf + pathsize, BUFSIZ - pathsize, 
			 ".%s", childtype->fieldname);
	  vm_inform_type_addr(childtype->name, recpath + 1, 
			      (elfsh_Addr) childata, newexpr, 0);
	  pathsize += len;

	  /* Insert child where necessary */ 
	  if (!revm_expr_init(recpath, newexpr, childtype, 
			      childata, newexpr->strval))
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Invalid child tree for variable", NULL);
	    }   
	    
	  pathsize -= len;
	  bzero(pathbuf + pathsize, len);
	}

      /* Terminal case : no recursion */
      else
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] NOW Terminal field, setting its value\n");
#endif

	  /* Handle RAW terminal field */
	  if (curtype->type == ASPECT_TYPE_RAW)
	    {
	      //XXX: Call hexa converter curval.datastr and set field
	      fprintf(stderr, " [E] Raw object initialization yet unsupported.\n");
	      continue;
	    }

	  /* Lookup scalar value and assign it to the field */
	  newexpr->value = vm_revmobj_lookup_real(curtype, recpath + 1, 
						  childtype->fieldname);
	  curdata  = vm_lookup_immed(newexpr->strval);
	  if (!newexpr->value || !curdata)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to lookup src or dst object", NULL);
	    }
	  if (vm_revmobj_set(newexpr->value, curdata) < 0)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Failed to set destination object", NULL);
	    }

	  /* Handle terminal Array fields */
	  if (curtype->dimnbr && curtype->elemnbr)
	    {
	      //XXX: Use previous code in loop
	      //XXX: Use child->elemnbr[idx] foreach size of dim
	      fprintf(stderr, 
		      " [E] Arrays objects initialization unsupported\n");
	      continue;
	    }
	}

      /* Link next field of current structure */
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
	    {
	      prevexpr = newexpr;
	      rootexpr = newexpr;
	    }
	}
    }

  /* Return success or error */
  if (!rootexpr)
    {
      if (newexpr)
	XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to find a root expression", NULL);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rootexpr);
}



/* Compare or Set source and destination */
/* OP = REVM_OP_SET or REVM_OP_CMP */
static int		revm_expr_cmpset(revmexpr_t	*dest, 
					 revmexpr_t	*source, 
					 u_char		op)
{
  int			ret;
  elfsh_Addr		cmpval;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!dest || !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  if (op != REVM_OP_SET && op != REVM_OP_CMP)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid requested operation", -1);

  /* Recursive comparison or setting */
  for (cmpval = 0; dest && source; dest = dest->next, source = source->next)
    if (dest->childs || source->childs)
      {
	ret = revm_expr_cmpset(dest->childs, source->childs, op);
	if (ret != 0)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
      }
    else switch (op)
      {
      case REVM_OP_SET:
	if (vm_revmobj_set(dest->value, source->value) < 0)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unable to set expression field", -1);
	break;
      case REVM_OP_CMP:	  
	if (vm_cmp(dest->value, source->value, &cmpval) < 0)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unable to compare expression fields", -1);
	if (cmpval)
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	break;
      }
  
  /* Final checks and result */
  if ((dest && !source) || (!dest && source))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Get (and optionally print) the tree of an expression */
static int	revm_expr_printrec(revmexpr_t *expr, u_int taboff, u_int typeoff)
{
  aspectype_t	*curtype;
  char		buf[BUFSIZ];
  int		len;
  char		*size;
  char		*typename;
  char		offset[128];
  int		idx;
  int		sz;
  char		*pad, *pad2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (; expr; expr = expr->next)
    {
      
      /* Get type of the first real top level expression */
      curtype = aspect_type_get_by_id(expr->typeid);
      typename = curtype->name;
      
      /* Some printing header */
      vm_endline();

      len = snprintf(buf, sizeof(buf), "%-18s %s %s", 
		     vm_colorfieldstr(typename),
		     vm_colortypestr_fmt("%s", expr->label), 
		     vm_colorfieldstr("= "));

      sz = (taboff < 21 ? 0 : len - vm_color_size(buf) - 20);
      pad = alloca(taboff + sz + 1);
      memset(pad, ' ', taboff + sz);
      pad[taboff + sz] = 0x00;
      
      /* If the type is a structure */
      if (curtype->childs)
	{
	  vm_output(buf);
	  vm_output(vm_colorfieldstr("{"));
	  vm_endline();
	  revm_expr_printrec(expr->childs, 
			     taboff + len - vm_color_size(buf) - 8, 
			     typeoff);
	  vm_output(vm_colorfieldstr("}"));
	  if (expr->next)
	    {
	      vm_output(",\n");
	      pad2 = alloca(taboff + 1);
	      memset(pad2, ' ', taboff);
	      pad2[taboff] = 0x00;
	      vm_output(pad2);
	    }

	  typeoff += curtype->size;
	  continue;
	}
      
      // FIXME-XXX: Not printed for now
      size = alloca(30);
      if (curtype->type == ASPECT_TYPE_RAW)
	snprintf(size, sizeof(size), "%s%s%s", 
		 vm_colorfieldstr("["), 
		 vm_colornumber("%u", curtype->size), 
		 vm_colorfieldstr("]"));
      
      
      // FIXME-XXX: Print up 10 elements of array ...
      else if (curtype->dimnbr && curtype->elemnbr)
	{
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += 20;
	  size = alloca(sz);
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += sprintf(size + sz, "%s%s%s", 
			  vm_colorfieldstr("["),
			  vm_colornumber("%u", curtype->elemnbr[idx]),
			  vm_colorfieldstr("]"));
	}
      else
	*size = 0x00;
      
      /* Format the offset */
      snprintf(offset, sizeof(offset), "@ off(%s)", 
	       vm_colornumber("%u", typeoff));
      
      /* Print field and next padding */
      vm_output(curtype->isptr ? "*" : "");
      vm_output(buf);
      vm_print_obj(expr->value);
      vm_output(offset);
      if (expr->next)
	{
	  vm_output(",\n");
	  vm_output(pad);      
	  typeoff += curtype->size;
	}
      vm_endline();
    }
      
  /* Return success  */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
  
/* Print an annotated expression */
int		revm_expr_print(char *pathname)
{
  revmexpr_t	*expr;
  int		ret;
  char		buf[BUFSIZ];
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  expr = hash_get(&exprs_hash, pathname + 1);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression name", -1);
  snprintf(buf, BUFSIZ, "  $%s %s", 
	   vm_colorfunction(pathname + 1),
	   vm_colorfieldstr("= {"));
  vm_output(buf);
  ret = revm_expr_printrec(expr, strlen(pathname) + 6, 0);
  vm_output(vm_colorfieldstr("}"));
  vm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Get an expression from its name */
revmexpr_t	*revm_expr_get(char *pathname)
{
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!pathname || *pathname != '$')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    
  expr = hash_get(&exprs_hash, pathname + 1);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression name", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}

/* Set an expression to the value of another (only if compatible) */
int		revm_expr_set(char *dest, char *source)
{
  revmexpr_t	*adst;
  revmexpr_t	*asrc;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dest || !source || *dest != '$' || *source != '$')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  adst = revm_expr_get(dest);
  asrc = revm_expr_get(source);
  if (!adst || !asrc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);
  ret = revm_expr_cmpset(adst, asrc, REVM_OP_SET);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Compare 2 typed expressions */
int		revm_expr_compare(char *original, char *candidate)
{
  revmexpr_t	*candid;
  revmexpr_t	*orig;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candidate || !original || *candidate != '$' || *original != '$')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  candid = revm_expr_get(candidate);
  orig   = revm_expr_get(original);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);
  ret = revm_expr_cmpset(candid, orig, REVM_OP_CMP);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Create a new revm expression */
revmexpr_t	*revm_expr_create(aspectype_t	*datatype,
				  char		*dataname,
				  char		*datavalue) 
{
  revmexpr_t	*expr;
  char		*data;
  char		*realname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dataname || *dataname != '$')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    
  XALLOC(__FILE__, __FUNCTION__, __LINE__, data, datatype->size, NULL);
  realname = dataname + 1;
  vm_inform_type_addr(datatype->name, realname, (elfsh_Addr) data, NULL, 0); 
  expr = revm_expr_init(dataname, NULL, datatype, data, datavalue);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to create REVMEXPR", NULL);    
  vm_inform_type_addr(datatype->name, realname, (elfsh_Addr) data, expr, 0); 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}
