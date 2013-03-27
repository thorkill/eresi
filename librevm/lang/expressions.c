/**
 * @file librevm/lang/expressions.c
 * @ingroup lang
 * @brief Implementation of scripting declarations for meta-language variables.
 *
 * Started on Jun 23 2007 23:39:51 jfv
 * $Id$
 */
#include "revm.h"


typedef struct s_exprcontext
{
  u_short	toplevel;
  u_short	pathsize;
  char		pathbuf[BUFSIZ + 1];  
  revmexpr_t	*curexpr;
  revmexpr_t	*prevexpr;
}		revmexprctx_t;


static revmexpr_t	*revm_expr_init(revmexprctx_t	*ctx,
					aspectype_t	*curtype, 
      					void		*srcdata,
					char		*datavalue);


/** Create a context for a (sub)expression initialization */
revmexprctx_t		*revm_expr_context_init(revmexpr_t *curexpr, revmexpr_t *prevexpr, 
						u_short toplevel, char *pathbuf)
{
  revmexprctx_t		*exprctx;
  u_int			len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  len = (pathbuf ? strlen(pathbuf) : 0);
  if (len > BUFSIZ)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid initial expression path", NULL);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, exprctx, sizeof(revmexprctx_t), NULL);
  exprctx->toplevel = toplevel;
  exprctx->pathsize = len;
  exprctx->curexpr  = curexpr;
  exprctx->prevexpr = prevexpr;
  if (pathbuf && len)
    memcpy(exprctx->pathbuf, pathbuf, len);
  exprctx->pathbuf[len] = 0x00;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, exprctx);
}


/* Remove an expression context */
void			revm_expr_context_destroy(revmexprctx_t *ctx)
{
  XFREE(__FILE__, __FUNCTION__, __LINE__, ctx);
}

/** Get the real value of the parameter string for further revmobj initialization */
/** This function set the label and strval fields of the newly created revmexpr_t */
/* FIXME: does not return correct type name if there are trailing whitespaces */
static revmexpr_t *revm_expr_read(char **datavalue)
{
  revmexpr_t	*expr;
  char		*datastr;
  u_int		opening;
  u_int		closing;
  u_int		beginning;
  char		*namend;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!datavalue || !(*datavalue))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", NULL);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] EXPR_READ INPUT =  %s \n", *datavalue);
#endif

  datastr = *datavalue;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);

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
      for (*datavalue = datastr--; *datastr == ')' && *namend == '(' && beginning-- > 0; 
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


/* Initialize a field for an ERESI expression */
static int		revm_expr_init_field(revmexprctx_t *ctx, aspectype_t *parenttype, void *srcdata)
{
  aspectype_t		*childtype;
  u_int			len;
  revmexpr_t		*curdata;
  void			*childata;
  char			*recpath;
  revmexpr_t		*newexpr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctx || !srcdata || !parenttype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", -1);

  newexpr = ctx->curexpr;
  recpath = ctx->pathbuf;
  childtype = newexpr->type;
  
#if __DEBUG_EXPRS__
  printf(" [D] expr_init_field: setting TERMINAL value (recpath = %s, newexpr = %p) \n", 
	 recpath, newexpr);
#endif
  
  /* Handle RAW terminal field */
  if (childtype->type == ASPECT_TYPE_RAW)			       
    {
      // XXX-FIXME: Call hexa converter curval.datastr and set field
      fprintf(stderr, " [E] Raw object initialization yet unsupported.\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    }
  
  /* Lookup scalar value and assign it to the field */
  newexpr->value = revm_object_lookup_real(parenttype, recpath, childtype->fieldname, 0);	
  curdata = revm_compute(newexpr->strval);
  if (!newexpr->value || !curdata)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      ctx->pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to lookup src or dst object", -1);
    }
  
  /* Convert source data to recipient type and set it */
  if (newexpr->type->type != curdata->type->type)
    revm_convert_object(curdata, newexpr->type->type);
  if (revm_object_set(newexpr, curdata) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      ctx->pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to set destination object", -1);
    }
  
  /* Destroy the temporary variable we have created for the right-hand-side value */
  if (revm_variable_istemp(curdata))
    revm_expr_destroy_by_name(curdata->label);
  
  /* Handle terminal Array fields */
  if (childtype->dimnbr && childtype->elemnbr)			
    {
      //FIXME: Use child->elemnbr[idx] foreach size of dim (Use previous code in loop)
      fprintf(stderr, 
	      " [E] Arrays objects initialization unsupported\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    }
  
  /* Inform the runtime system about this terminal field */
  childata = (char *) srcdata + childtype->off;			
  len = snprintf(recpath + ctx->pathsize, BUFSIZ - ctx->pathsize,		
		 ".%s", childtype->fieldname);			
  revm_inform_type_addr(childtype->name, strdup(recpath),		
			(eresi_Addr) childata, newexpr, 0, 0); 
  bzero(recpath + ctx->pathsize, len);				
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Prepare recursion on ERESI expression initialisation */
//static int	revm_expr_init_rec(revmexpr_t **newexpr, char *pathbuf, char *recpath, u_int *pathsize, void *srcdata)
static int	revm_expr_init_rec(revmexprctx_t *ctx, void *srcdata)
				   
{
  void		*childata;
  u_int		len;
  aspectype_t	*childtype;
  revmexpr_t	*expr;
  revmexpr_t	*newexpr;
  revmexpr_t	*backup;
  char		*recpath;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctx || !srcdata)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", -1);

  newexpr = ctx->curexpr;
  recpath = ctx->pathbuf;
  childtype = newexpr->type;

  /* If the substructure is initialized with the value of another expression, no need to
     do any additional read, init or inform on the current field */
  if (newexpr->strval[0] == REVM_VAR_PREFIX)
    {
      expr = revm_expr_get(newexpr->strval);
      if (!expr)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	  ctx->pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unable to get root field or type name", -1);
	}
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] expr_init_rec: FOUND EXISTING INITIAL EXPR = %s :: %s "
	      "RECPATH = %s (now doing copy) \n", expr->label, expr->strval, recpath);
#endif
		
      len = snprintf(recpath + ctx->pathsize, BUFSIZ - ctx->pathsize, ".%s", newexpr->label);
      newexpr = revm_expr_copy(expr, recpath, 1);
      ctx->curexpr = newexpr;
      bzero(recpath + ctx->pathsize, len);
      if (!newexpr)
	{
	  ctx->pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unable to copy existing initial subexpression", -1);
	}
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] COPIED EXISTING INITIAL REVMEXPR = %s :: %s (new name) \n", 
	      newexpr->label, newexpr->strval);
#endif
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* No initial variable value, initialize with immediates */
#if __DEBUG_EXPRS_MORE__
  fprintf(stderr, " [D] RECORD field -- recursing ! \n");
#endif
  
  childata = (char *) srcdata + childtype->off;
  len = snprintf(recpath + ctx->pathsize, BUFSIZ - ctx->pathsize, ".%s", childtype->fieldname);
  revm_inform_type_addr(childtype->name, strdup(recpath), (eresi_Addr) childata, newexpr, 0, 0);
  
  /* Insert child where necessary */ 
  ctx->pathsize += len;
  backup = ctx->prevexpr;
  ctx->prevexpr = NULL;
  if (!revm_expr_init(ctx, childtype, childata, newexpr->strval))		      
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      ctx->pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid child tree for variable", -1);
    }   
  ctx->prevexpr = backup;
  ctx->pathsize -= len;
  bzero(recpath + ctx->pathsize, len);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Prepare the new expression for initialization */
static revmexpr_t	*revm_expr_preinit(revmexprctx_t *ctx, aspectype_t *curtype, char **datavalue)
{
  revmexpr_t		*newexpr;
  aspectype_t		*childtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!ctx || !curtype || !datavalue || !(*datavalue))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", NULL);	  

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Current datavalue = (%s) \n", *datavalue);
#endif
  
  /* Read next typed expression in string */
  newexpr = revm_expr_read(datavalue);
  if (newexpr == NULL || newexpr->strval == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to read expression value", NULL);	  
  if (!ctx->toplevel && !newexpr->label)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to read expression label", NULL);	  


  /* If we are at the root expression and that type was explicitly given, perform type checking */
  if (ctx->toplevel && newexpr->label)
    {
      
#if __DEBUG_EXPRS_MORE__
      fprintf(stderr, " [D] Top level type with explicit type label = (%s) strval = (%s) \n", 
	      newexpr->label, newexpr->strval);
#endif
      
      if (strcmp(newexpr->label, curtype->name))
	{
	  ctx->pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Type checking failed during expression initialization", NULL);	  
	}
      *datavalue = newexpr->strval;
      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      ctx->toplevel = 0;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (revmexpr_t *) -1);
    }
  
  /* Implicit type for this expression -- no type checking */
  else if (ctx->toplevel && !newexpr->label)
    {
      ctx->toplevel = 0;
      *datavalue = newexpr->strval;
      
#if __DEBUG_EXPRS_MORE__
      fprintf(stderr, " [D] No explicit top-level type, continuing with datavalue = %s \n", 
	      *datavalue);
#endif
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (revmexpr_t *) -1);
    }

  ctx->toplevel = 0;
  childtype = aspect_type_get_child(curtype, newexpr->label);
  if (!childtype)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      ctx->pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid child structure for variable", NULL);
    }
  newexpr->type = childtype;
  
  /* Duplicate names cause they are on the stack now */
  newexpr->label = (char *) strdup(newexpr->label);
  newexpr->strval = (char *) strdup(newexpr->strval);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newexpr);
}





/* Initialize an ERESI expression */
static revmexpr_t	*revm_expr_init(revmexprctx_t	*ctx,
					aspectype_t	*parenttype, 
    					void		*srcdata,
					char		*datavalue)
{
  revmexpr_t	*newexpr, *rootexpr, *backupexpr;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctx || !parenttype || !srcdata || !datavalue)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", NULL);	  

  backupexpr = newexpr = rootexpr = NULL;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Entering revm_expr_init with toplevel = %u , curpath = %s\n", 
	  ctx->toplevel, ctx->pathbuf);
#endif

  /* Construct the expression until end of ascii string */
  while (*datavalue)
    {
      newexpr = revm_expr_preinit(ctx, parenttype, &datavalue);
      if (!newexpr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to pre-initialize sub-expression", NULL);
      else if ((int) newexpr == -1) { newexpr = NULL; continue; }

      backupexpr = ctx->curexpr;
      ctx->curexpr = newexpr;
      
      /* Non-terminal case : we will need to recurse */
      if (newexpr->type->childs && !newexpr->type->dimnbr)
	ret = revm_expr_init_rec(ctx, srcdata);
      /* Terminal case : no recursion */
      else
	ret = revm_expr_init_field(ctx, parenttype, srcdata);

      ctx->curexpr = backupexpr;

      if (ret == -1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to initialize expression field", NULL);
      else if (ret == 1) continue;
      
      /* Link next field of current structure */
      newexpr->parent = ctx->curexpr;
      if (ctx->curexpr)
	{
	  rootexpr = ctx->curexpr;
	  if (ctx->prevexpr)
	    {
	      ctx->prevexpr->next = newexpr;
	      ctx->prevexpr = newexpr;
	    }
	  else
	    {
	      ctx->curexpr->childs = newexpr;
	      ctx->prevexpr = newexpr;
	    }
	}
      else
	{
	  if (ctx->prevexpr)
	    {
	      ctx->prevexpr->next = newexpr;
	      ctx->prevexpr = newexpr;
	    }
	  else
	    rootexpr = ctx->prevexpr = newexpr;
	}		
    }
  
  /* Return success or error */
  if (!rootexpr)
    {
      if (!newexpr)
	{
	  ctx->pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unable to find a root expression", NULL);
	}
      rootexpr = newexpr;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rootexpr);
}


/* This function is called when the LHS is an unknown variable which has to be created */
/* When the command has a LHS of the form "set $base.field val" and $base has no current 
value for $field. If the DSTNAME parameter starts with a variable prefix and contains 
a field delimiter, we extract the variable name and the field path. If such base ariable
exists and the (remaining) field name is valid and passes type-checking, we create a new field 
value for this instance */
revmexpr_t		*revm_expr_extend(char *dstname, char *srcvalue)
{
  revmexpr_t		*expr = NULL;
  char			*nextfield;
  aspectype_t		*fieldtype;
  char			*newdata;
  revmexprctx_t		*ctx;
  revmexpr_t		*prev;
  char			*limit;
  char			*dstcopy;
  char			valuestr[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Find longest valid prefix */
  if (*dstname != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to find valid expression prefix", NULL);
  nextfield = dstcopy = strdup(dstname);
  do {
    limit = strchr(nextfield, REVM_SEP[0]);
    if (limit)
      {
	*limit = 0x00;
	expr = revm_expr_get(dstcopy);
	if (expr)
	  {
	    *limit = REVM_SEP[0];
	    nextfield = limit + 1;
	  }
      }
  }
  while (limit);
  if (!expr)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, dstcopy);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to find valid expression prefix", NULL);
    }
  if (nextfield > dstcopy)
    nextfield[-1] = 0x00;

  /* Check if field exits in parent type, initialize it if yes */
  fieldtype = aspect_type_get_child(expr->type, nextfield); 
  if (!fieldtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid field extension", NULL);

  /* Setup context and call expression init! */
  for (prev = expr->childs; prev && prev->next; prev = prev->next);
  ctx = revm_expr_context_init(expr, prev, 0, dstcopy);
  if (!ctx)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to allocate context", NULL);

  newdata = (char *) expr->annot->addr;

  snprintf(valuestr, sizeof(valuestr), "%s(%s)", nextfield, srcvalue);
  expr = revm_expr_init(ctx, expr->type, newdata, valuestr);
  revm_expr_context_destroy(ctx);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to initialize expression extension", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}


/* Compare or Set source and destination */
/* OP = REVM_OP_SET or REVM_OP_MATCH */
static int		revm_expr_handle(revmexpr_t	*dest, 
					 revmexpr_t	*source, 
					 u_char		op)
{
  int			ret;
  eresi_Addr		cmpval;
  revmexpr_t		*cursource;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!dest || !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  if (op != REVM_OP_SET && op != REVM_OP_MATCH)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid requested operation", -1);

  /* Recursive comparison or setting */
  for (cmpval = 0; dest; dest = dest->next)
    {

      /* Find this field in the current hierarchy list of fields */
      for (cursource = source; cursource; cursource = cursource->next)
	if (!strcmp(dest->label, cursource->label))
	  break;
      if (!cursource)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot find expression field", -1);
      
      if (cursource->childs)
	{
	  ret = revm_expr_handle(dest->childs, cursource->childs, op);
	  if (ret != 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
	  continue;
	}

      /* FIXME-XXX: set/match in case of array ! */
      /* FIXME-XXX: set/match in case of raw type ! */
      
      /* Now realize the operation depending if we are a leaf or not */
      switch (op)
	{
	case REVM_OP_SET:
	  if ((ret = revm_object_set(dest, cursource)) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to set expression field", ret);
	  break;
	case REVM_OP_MATCH:	  
	  if ((ret = revm_object_compare(dest, cursource, &cmpval)) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to compare expression fields", ret);
	  if (cmpval)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	  break;
	}
    }
  
  /* Final checks and result */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Get (and optionally print) the tree of an expression */
static int	revm_expr_printrec(revmexpr_t *expr, u_int taboff, 
				   u_int typeoff, u_int iter, u_char quiet)
{
  aspectype_t	*curtype;
  char		buf[BUFSIZ];
  int		len;
  char		*size;
  char		*typename;
  char		offset[128];
  int		sz;
  char		*pad, *pad2;
  int		newtaboff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (; expr; expr = (iter ? expr->next : NULL))
    {
      
      /* Get type of the first real top level expression */
      curtype = expr->type;
      typename = curtype->name;
      
      /* Some printing header */
      revm_endline();
      len = snprintf(buf, sizeof(buf), "%s%*s %*s", 
		     (curtype->isptr ? (quiet ? " *" : revm_colorwarn("    *")) : ""),
		     (iter && !quiet ? 18 - (curtype->isptr * 4) : 0), 
		     (quiet ? "" : revm_colorfieldstr(typename)), 
		     (iter && !quiet ? 18 : 0), 
		     (quiet ? expr->label : revm_colortypestr_fmt("%10s", expr->label)));
      
      sz = (taboff < 21 ? 0 : (len - revm_color_size(buf) - 20));
      pad = alloca(taboff + sz + 1);
      memset(pad, ' ', taboff + sz);
      pad[taboff + sz] = 0x00;
      
      /* If the expr is a structure */
      if (expr->childs)
	{
	  revm_output(buf);
	  if (!quiet)
	    revm_output(revm_colorwarn(" {"));
	  else
	    revm_output("(");
	  revm_endline();
	  newtaboff = taboff + len - revm_color_size(buf) - 7;
	  revm_expr_printrec(expr->childs, (newtaboff < 0 ? 1 : newtaboff),
			     typeoff, iter, quiet);
	  if (!quiet)
	    revm_output(revm_colorwarn("}"));
	  else
	    revm_output(")");
	  if (expr->next)
	    {
	      if (quiet)
		revm_output(",");
	      else
		revm_output(",\n");
	      pad2 = alloca(taboff + 1);
	      memset(pad2, ' ', taboff);
	      pad2[taboff] = 0x00;
	      revm_output(pad2);
	    }

	  /* Do not add offset if we are in union */
	  if (expr->next && expr->next->type->off != curtype->off)
	    typeoff += (expr->type->isptr ? 4 : curtype->size);
	  continue;
	}
      
      // FIXME-XXX: Print up to 10 bytes
      if (curtype->type == ASPECT_TYPE_RAW)
	{
	  size = alloca(100);
	  snprintf(size, 100, "%s%s%s", 
		   revm_colorwarn("["), 
		   revm_colornumber("%u", curtype->size), 
		   revm_colorwarn("] = {...} "));
	}
      
      // FIXME-XXX: Print up 10 elements of array ...
      else if (curtype->dimnbr && curtype->elemnbr)
	{
	  /*
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += 40;
	  size = alloca(sz + 50);
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += sprintf(size + sz, "%s%s%s", 
			  revm_colorwarn("["),
			  revm_colornumber("%u", curtype->elemnbr[idx]),
			  revm_colorwarn("]"));
	  */
	  size = alloca(50);
	  sprintf(size, "%s", revm_colorwarn(" = "));
	}
      else
	{
	  size = alloca(50);
	  snprintf(size, 50, "%s", revm_colorwarn(" = "));
	}
      
      /* Print everything in order */
      revm_output(buf);
      revm_output(size);
      if (expr->value)
	revm_object_print(expr->value);

      /* Next field ! Print offset .. */
      if (iter && expr->next)
	{
	  if (!quiet)
	    {
	      snprintf(offset, sizeof(offset), "@ off(%s)", revm_colornumber("%u", typeoff));
	      revm_output(offset);	      
	      revm_output(revm_colorwarn(",\n"));
	      revm_output(pad);      
	    }
	  else
	    revm_output(", ");

	  /* Do not add size if we are in a union */
	  if (expr->next->type->off != curtype->off)
	    typeoff += (curtype->isptr ? 4 : curtype->size);
	}
      revm_endline();
    }
      
  /* Return success  */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Recursive copy of an expression */
static int		revm_expr_copyrec(revmexpr_t    *parent,
					  revmexpr_t	*dest, 
					  revmexpr_t	*source,
					  char		*newname,
					  u_int		namelen,
					  u_int		nameoff,
					  char		*data)
{
  int			ret;
  aspectype_t		*type;
  char			*childata;
  u_int			len;

  /* Preliminary checks */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dest || !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

  /* Recursive comparison or setting */
  while (source)
    {
      memcpy(dest, source, sizeof(revmexpr_t));
      if (source->label)
	dest->label = strdup(source->label);
      if (source->strval)
	dest->strval = strdup(source->strval);
      dest->parent = parent;
      
      /* Copy children structure */
      if (source->childs)
	{
	  type = source->type;
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->childs, sizeof(revmexpr_t), -1);
	  len = snprintf(newname + nameoff, namelen - nameoff, ".%s", source->label);
	  childata = data + type->off;
	  revm_inform_type_addr(type->name, strdup(newname), (eresi_Addr) childata, dest, 0, 0);
	  ret = revm_expr_copyrec(dest, dest->childs, source->childs, newname, 
				  namelen, nameoff + len, childata);
	  if (ret != 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
	  bzero(newname + nameoff, len);
	}

      /* FIXME-XXX: copy in case of array ! */
      /* FIXME-XXX: copy in case of raw type ! */

      /* Copy terminal field */
      else if (source->value)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->value, sizeof(revmobj_t), -1);
	  type = source->value->otype;
	  dest->value = revm_object_lookup_real(parent->type, newname, source->label, 0);
	  if (!dest->value)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, dest);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Failed to lookup terminal object", -1);
	    }
	  len = snprintf(newname + nameoff, namelen - nameoff, ".%s", source->label);
	  childata = data + type->off;
	  revm_inform_type_addr(type->name, strdup(newname), (eresi_Addr) childata, dest, 0, 0);
	  bzero(newname + nameoff, len);
	  if (revm_object_set(dest, source) < 0)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, dest);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Failed to copy terminal object", -1);
	    }
	}

      /* Allocate next element */
      source = source->next;
      if (source)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->next, sizeof(revmexpr_t), -1);
	  dest = dest->next;
	}
    }
  
  /* Final checks and result */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Copy an expression (set $e1 $e2) */
revmexpr_t	*revm_expr_copy(revmexpr_t *source, char *dstname, u_char isfield)
{
  revmexpr_t	*dest;
  aspectype_t	*type;
  int		ret;
  char		newname[BUFSIZ] = {0x00};
  int		curoff;
  char		*copydata;
  revmannot_t	*annot;
  char		*str, *str2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First copy the data */
  type = source->type;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Allocating copied expression data of size %u type %s to name %s (%s)\n", 
	  type->size, type->name, dstname, (isfield ? "field" : "struct"));
#endif

  XALLOC(__FILE__, __FUNCTION__, __LINE__, copydata, type->size, NULL);

  /* Constants are not annotated, we might not want to do anything here */
  annot = revm_annot_get(source->label);
  if (annot)
    memcpy(copydata, (char *) annot->addr, type->size);

  /* Create a temporary variable if necessary (for constants) */
  if ((!isfield && *dstname != REVM_VAR_PREFIX) || 
      !strncmp(dstname, "$hash", 5) || 
      !strncmp(dstname, "$list", 5))
    dstname = revm_tmpvar_create();
  else
    dstname = strdup(dstname);

  /* Then copy the expression meta-data */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest, sizeof(revmexpr_t), NULL);
  strncpy(newname, dstname, sizeof(newname));
  curoff = strlen(newname);
  if (source->strval)
    dest->strval = strdup(source->strval);

  /* If we are copying a subexpression, make sure labels are copied relative */
  if (isfield)
    {
      for (str = newname; 
	   str && *str && (str2 = strchr(str, '.')); 
	   str = str2 + 1);
      if (!str || !*str)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to inform copy expression", NULL);
    }
  else
    str = newname;
  dest->label    = strdup(str);
  dest->type     = type;
  if (!revm_inform_type_addr(type->name, strdup(dstname), (eresi_Addr) copydata, dest, 0, 0))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to inform copy expression", NULL);

  /* Simple object copy is easier */
  if (source->value)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->value, sizeof(revmobj_t), NULL);
      dest->value->otype = dest->type;
      dest->value->immed = 1;
      dest->value->perm  = 1;
      ret = revm_object_set(dest, source);
    }

  /* Some (structure) expressions have an extra top-level type, copy was done before the recursion */
  else if (source->childs) 
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->childs, sizeof(revmexpr_t), NULL);
      ret = revm_expr_copyrec(dest, dest->childs, source->childs, 
			      newname, BUFSIZ, curoff, copydata);
    }

  /* Failure: no value or complex type for source expr ? */
  else
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to copy invalid expression (childs = NULL, value = NULL)", NULL);
    }

  /* Return error or success */
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to copy expression", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}


/* Create an expression from an object */
revmexpr_t	*revm_expr_create_from_object(revmobj_t *copyme, char *name, u_char force)
{
  revmexpr_t	*dest;
  aspectype_t	*type;
  void		*data;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!copyme)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameter", NULL);
  if (force)
    {
      dest = revm_expr_get(name);
      if (dest)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
    }

  /* Create a new temporary variable name if necessary */
  if (*name != REVM_VAR_PREFIX || 
      !strncmp(name, "$hash", 5) || 
      !strncmp(name, "$list", 5))
    name = revm_tmpvar_create();
  else
    name = strdup(name);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest, sizeof(revmexpr_t), NULL);
  dest->type     = copyme->otype;
  type           = dest->type;
  dest->label    = name;
  if (!copyme->otype || (aspect_type_simple(copyme->otype->type) && !copyme->otype->next))
    dest->value  = copyme;
  else
    {
      data = (void *) copyme->get_obj(copyme->parent);
      if (!data)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to dereference object", NULL);
      if (!revm_inform_type_addr(type->name, strdup(name), (eresi_Addr) data, dest, 0, 1))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to create expression from complex object", NULL);
    }

  hash_add(&world.curjob->recur[world.curjob->curscope].exprs, 
	   (char *) strdup(name), (void *) dest);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Create_Expr_From_Object %s added with type = %s \n", 
	  name, (type ? type->name : "UNKNOWN TYPE"));
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}

  
/* Print an annotated expression */
int		revm_expr_print(revmexpr_t *expr, u_char quiet)
{
  int		ret;
  char		buf[BUFSIZ];
  int		iter;
  int		uniq;
  aspectype_t	*type;
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!expr || !expr->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);

  /* Make sure we only print the subrecord if requested */
  type = expr->type;
  name = expr->label;
  uniq = 0;
  if (expr->childs)
    {
      expr = expr->childs;
      iter = 1;
      uniq = 1;
    }
  else
    iter = (aspect_type_simple(expr->type->type) ? 0 : 1);

  /* If we are printing a simple type or a subtype expression, we need to print a top level */
  if (uniq || type->next || (type->childs && type->childs->next))
    {
      if (!quiet)
	snprintf(buf, BUFSIZ, " %s %s \t %s", 
		 revm_colorfunction(type->name),
		 revm_colorfunction(name), revm_colorwarn("= {"));
      else
	snprintf(buf, BUFSIZ, " %s(", 
		 revm_colorfunction(type->name));
      revm_output(buf);
      revm_endline();
    }

  ret = revm_expr_printrec(expr, (!iter || !expr->next ? strlen(name) + 6 : 1), 0, iter, quiet);

  if (!quiet && (uniq || type->next || (type->childs && type->childs->next)))
    revm_output(revm_colorwarn("}"));
  else if (quiet)
    revm_output(")\\l");

  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Print an annotated expression */  
int		revm_expr_print_by_name(char *pathname, u_char quiet)
{
  revmexpr_t	*expr;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!pathname || *pathname != REVM_VAR_PREFIX)
    {
      fprintf(stderr, "FAILED EXPR NAME %s:\n", pathname);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid name for expression", -1);    
    }
  expr = revm_expr_get(pathname);
  if (!expr || !expr->type)
    {
      fprintf(stderr, "FAILED EXPR %p TYPE \n", expr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unknown expression name", -1);
    }
  ret = revm_expr_print(expr, quiet);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/** Get an expression from its name */
revmexpr_t	*revm_expr_get(char *pathname)
{
  int		index;
  hash_t	*curhash;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!pathname || *pathname != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    

  for (index = world.curjob->curscope; index >= 0; index--)
    {
      curhash = &world.curjob->recur[index].exprs;
      expr = hash_get(curhash, pathname);      
      if (expr)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
    }
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid expression name", NULL);
}

/** Set an expression to the value of another (only if compatible) */
int		revm_expr_set_by_name(char *dest, char *source)
{
  revmexpr_t	*adst;
  revmexpr_t	*asrc;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dest || !source || *dest != REVM_VAR_PREFIX || *source != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  adst = revm_expr_get(dest);
  asrc = revm_expr_get(source);
  if (!adst || !asrc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);

  /* Return result */
  ret = revm_expr_set(adst, asrc);
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to set expression by name", ret);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Set an expression to the value of another (only if compatible) */
int		revm_expr_set(revmexpr_t *adst, revmexpr_t *asrc)
{
  int		ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!adst || !asrc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);    
  
  /* Take care of top-level structure types */
  if (asrc->childs)
    asrc = asrc->childs;
  if (adst->childs)
    adst = adst->childs;

  /* Necessary for assignment of scalar values */
  if (adst->value && asrc->value && (!asrc->next || !adst->next))
    {
      /* See if object conversion is necessary and/or possible */
      if ((ret = revm_nextconds_atomics(adst, asrc)) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to set scalar variable", -1);
      if ((ret = revm_object_set(adst, asrc)) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to set scalar variable", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Filter the base error case */
  else if (adst->value && !asrc->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot set scalar with complex value", ret);
  else if (!adst->value && asrc->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot set structure with scalar value", ret);

  /* Set a whole structure or a substructure */
  else
    ret = revm_expr_handle(adst, asrc, REVM_OP_SET);

  /* Result */
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to set expression", ret);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Compare 2 typed expressions */
int		revm_expr_compare_by_name(char *original, char *candidate, eresi_Addr *val)
{
  revmexpr_t	*candid;
  revmexpr_t	*orig;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candidate || !original || *candidate != REVM_VAR_PREFIX || *original != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  candid = revm_expr_get(candidate);
  orig   = revm_expr_get(original);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);

  ret = revm_expr_compare(orig, candid, val);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Compare 2 typed expressions */
int		revm_expr_compare(revmexpr_t *orig, revmexpr_t *candid, eresi_Addr *val)
{
  int		ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Take care of top-level structure types */
  if (orig->childs)
    orig = orig->childs;
  if (candid->childs)
    candid = candid->childs;

  /* Comparison between simple objects */
  /* Necessary for comparison with constants without a tree/field structure */
  if (orig->value && candid->value && !orig->next && !candid->next)
    {
      if ((ret = revm_object_compare(orig, candid, val)) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid name for expression", -1);    
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Filter the base error case */
  else if ((orig->value && !candid->value) || (!orig->value && candid->value))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot compare scalar with complex expression", ret);

  /* Comparison between whole structures */
  /* A comparison is an symmetric matching of the 2 objects */
  else
    {
      ret = revm_expr_handle(candid, orig, REVM_OP_MATCH);
      if (!ret)
	ret = revm_expr_handle(orig, candid, REVM_OP_MATCH);
    }

  /* Return result */
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Failed to compare expressions", ret);
  *val = ret;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Match or not 2 typed expressions */
int		revm_expr_match_by_name(char *original, char *candidate)
{
  revmexpr_t	*candid;
  revmexpr_t	*orig;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candidate || !original || *candidate != REVM_VAR_PREFIX || *original != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  candid = revm_expr_get(candidate);
  orig   = revm_expr_get(original);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);
  ret = revm_expr_match(candid, orig);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Match or not 2 typed expressions */
int		revm_expr_match(revmexpr_t *candid, revmexpr_t *orig)
{
  int		ret  = 0;
  eresi_Addr	*val = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);    

  /* Take care of top-level structure types */
  if (!orig->next && orig->childs)
    orig = orig->childs;
  if (!candid->next && candid->childs)
    candid = candid->childs;

  /* For simple objects */
  if (orig->value && candid->value && !orig->next && !candid->next)
    {
      if ((ret = revm_object_compare(orig, candid, val)) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid name for expression", -1);    
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Comparison between whole structures */
  /* A comparison is an symmetric matching of the 2 objects */
  else if ((orig->value && !candid->value) || (!orig->value && candid->value))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot match scalar with complex expression", ret);

  /* Comparison between substructures */
  else
    ret = revm_expr_handle(candid, orig, REVM_OP_MATCH);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/* Create a new revm expression */
revmexpr_t	*revm_expr_create(aspectype_t	*datatype,
				  char		*dataname,
				  char		*datavalue) 
{
  revmexpr_t	*expr;
  revmexpr_t	*source;
  char		*databuff;
  char		*realname;
  revmexprctx_t	*exprctx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dataname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    
  if (!datatype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for expression", NULL);    

  /* Create a new temporary if necessary */
  if (*dataname != REVM_VAR_PREFIX || 
      !strncmp(dataname, "$hash", 5) || 
      !strncmp(dataname, "$list", 5))
    dataname = revm_tmpvar_create();
  else
    dataname = strdup(dataname);

  /* In case the source value is an existing expression, lets find it now */
  source = revm_expr_get(datavalue);
  if (source)
    {
      expr = revm_expr_copy(source, dataname, 0);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
    }
  
  /* Else we create and initialize a new expression */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, databuff, datatype->size, NULL);
  realname = dataname;
  revm_inform_type_addr(datatype->name, strdup(realname), (eresi_Addr) databuff, NULL, 0, 0);
  
  exprctx = revm_expr_context_init(NULL, NULL, 1, dataname);
  if (!exprctx)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to allocate context", NULL);
  if (!datatype->next && datatype->childs)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);
      expr->strval = strdup(datavalue);
      expr->label  = strdup(dataname);
      expr->type   = datatype;
      expr->childs = revm_expr_init(exprctx, datatype, databuff, datavalue);
    }
  else
    expr = revm_expr_init(exprctx, datatype, databuff, datavalue);
  
  revm_expr_context_destroy(exprctx);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to create REVMEXPR", NULL);    

  revm_inform_type_addr(datatype->name, strdup(realname), (eresi_Addr) databuff, expr, 0, 0);

#if __DEBUG_EXPRS__
  revm_expr_print_by_name(expr->label, 0);
  revm_output("\n");
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}


/* Simply create an expression from a constant value */
revmexpr_t	*revm_simple_expr_create(aspectype_t *datatype, char *name, char *value)
{
  revmexpr_t	*expr;
  revmobj_t	*obj;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);
  if (value)
    obj = revm_lookup_immed(value, 1);
  else
    obj = revm_object_lookup_real(datatype, name, NULL, 0);
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to create REVMEXPR", NULL);
  
  if (*name != REVM_VAR_PREFIX || !strncmp(name, "$hash", 5) || !strncmp(name, "$list", 5))
    name = revm_tmpvar_create();
  else
    name = strdup(name);

  expr->type   = datatype;
  expr->strval = (value ? strdup(value) : NULL);
  expr->value  = obj;
  expr->label  = name;
  hash_add(&world.curjob->recur[world.curjob->curscope].exprs, 
	   (char *) strdup(name), (void *) expr);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] SimpleExpr %s added with type = %s \n", 
	  name, (datatype ? datatype->name : "UNKNOWN TYPE"));
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}


/* Read the requested type for an expression in ascii form */
aspectype_t	*revm_exprtype_get(char *exprvalue)
{
  aspectype_t	*type;
  char		*typename;
  u_int		typenamelen;
  char		*curexprvalue;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!exprvalue)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", NULL);
  expr = revm_expr_get(exprvalue);
  if (expr)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr->type);

  type = NULL;
  for (curexprvalue = exprvalue, typenamelen = 0; *curexprvalue != '('; typenamelen++)
    curexprvalue++;
  typename = alloca(typenamelen + 1);
  strncpy(typename, exprvalue, typenamelen);
  typename[typenamelen] = 0x00;
  type = aspect_type_get_by_name(typename);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression type", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

/** Erase the expression from hash tables */
static int	revm_expr_erase(revmexprctx_t *ctx)
{
  int		index;
  revmexpr_t	*curexpr;
  hash_t	*thash;
  char		newname[BUFSIZ];
  revmexpr_t	*prevexpr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Remove expression from the most inner scope */
  for (index = world.curjob->curscope; index >= 0; index--)
    {
      curexpr = hash_get(&world.curjob->recur[index].exprs, ctx->pathbuf);
      if (curexpr)
	{
	  if (hash_del(&world.curjob->recur[index].exprs, ctx->pathbuf))
	    {
#if __DEBUG_EXPRS__
	      fprintf(stderr, "\n [D] FAILED TO UNLINK EXPR %s from recur[%u].exprs\n", 
		      ctx->pathbuf, index);
#endif
	    }
	  else
	    {
#if __DEBUG_EXPRS__
	      fprintf(stderr, "\n [D] UNLINKED EXPR %s from recur[%u].exprs\n", 
		      ctx->pathbuf, index);
#endif
	    }	  
	  break;
	}
    }
  /* Update the global type hashes too by recovering a previous expression of that name, if any */
  if (ctx->curexpr->type)
    {
      snprintf(newname, sizeof(newname), "type_%s", ctx->curexpr->type->name);
      thash = hash_find(newname);
      if (!thash)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot find hash table for this expression type", -1);
      if (hash_del(thash, ctx->pathbuf))
	{
#if __DEBUG_EXPRS__
	  fprintf(stderr, "\n [D] FAILED TO UNLINK EXPR %s from hash[type_%s] \n", 
		  ctx->pathbuf, ctx->curexpr->type->name);
#endif
	}
      else
	{
#if __DEBUG_EXPRS__
	  fprintf(stderr, "\n [D] UNLINKED EXPR %s from hash[type_%s] \n", 
		  ctx->pathbuf, ctx->curexpr->type->name);
#endif
	}
      
      prevexpr = revm_expr_get(ctx->pathbuf);
      if (prevexpr)
	{
#if __DEBUG_EXPRS__
	  fprintf(stderr, "\n [D] RESTORED EXPR %s (type %s) after UNSHADOWING UNLINK\n", 
		  ctx->pathbuf, ctx->curexpr->type->name);
#endif
	  
	  // In case previous (shadowed) variable with same name was not of the same type
	  if (prevexpr->type->type != ctx->curexpr->type->type)	
	    {
	      snprintf(newname, sizeof(newname), "type_%s", prevexpr->type->name);
	      thash = hash_find(newname);
	      if (!thash)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			     "Cannot find type hash for shadowed expression type", -1);
	    }	  
	  hash_set(thash, strdup(ctx->pathbuf), prevexpr->annot);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Unlink an expression */
static int	revm_expr_unlinkrec(revmexprctx_t *ctx, u_char exprfree, u_char datafree)
{
  revmexpr_t	*next;
  revmexpr_t	*supernext;
  revmexpr_t	*backupexpr;
  int		len;
  u_char	removeme;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctx || !ctx->curexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL context/curexpr parameter", -1);
  for (next = ctx->curexpr; next; next = supernext)
    {
      supernext = next->next; 
#if __DEBUG_EXPRS__
      fprintf(stderr, "New loop iter on ->next with pathbuf = %s (will extend) \n", ctx->pathbuf);
#endif
      len = snprintf(ctx->pathbuf + ctx->pathsize, sizeof(ctx->pathbuf) - ctx->pathsize,
		     ".%s", next->label);
      ctx->pathsize += len;
#if __DEBUG_EXPRS__
      fprintf(stderr, "Now with pathbuf = %s (extended) \n", ctx->pathbuf);
#endif
      backupexpr = ctx->curexpr;
      ctx->curexpr = next;
      ret = revm_expr_erase(ctx);
      ctx->curexpr = backupexpr;

      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to find expression in scope", -1);
      removeme = (((next->annot && next->annot->inhash) || exprfree == 0) ? 0 : 1);
      if (!removeme)
	{
#if __DEBUG_EXPRS__
	  fprintf(stderr, 
		  " [D] NOT FREED EXPR %s because still in hash table (exprfree = %u, datafree = %u) \n", 
		  ctx->pathbuf, exprfree, datafree);
#endif
	  ctx->pathsize -= len;
	  bzero(ctx->pathbuf + ctx->pathsize, len);
	  continue;
	}
      // Recurse on children!
      if (next->childs)
	{
#if __DEBUG_EXPRS__
	  fprintf(stderr, " [D] UNLINK RECURSION FOR %s (exprfree = %u, datafree = %u) \n", 
		  ctx->pathbuf, exprfree, datafree);
#endif 
	  backupexpr = ctx->curexpr;
	  ctx->curexpr = next->childs;
	  if (revm_expr_unlinkrec(ctx, exprfree, datafree) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Failed to destroy child expression", -1);
	  ctx->curexpr = backupexpr;
	}
      /* If the expression in present in some hash table, we dont free it */
#if __DEBUG_EXPRS_MORE__
      fprintf(stderr, "\n [D] UNLINK EXPR %s (exprfree = %hhu) \n", ctx->pathbuf, exprfree);
#endif

      if (next->value)
	revm_destroy_object(next->value, datafree);
      XFREE(__FILE__, __FUNCTION__, __LINE__, next);
      ctx->pathsize -= len;
      bzero(ctx->pathbuf + ctx->pathsize, len);

#if __DEBUG_EXPRS_MORE__
      fprintf(stderr, "\n [D] Came back to pathbuf = %s \n", ctx->pathbuf);
#endif

    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Unlink an expression : top level */
static int	revm_expr_unlink(revmexprctx_t *ctx, u_char exprfree, u_char datafree)
{
  int		ret;
  revmexpr_t	*backupexpr;
  u_char	removeme;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ctx || !ctx->curexpr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL context/curexpr parameter", -1);
  if (ctx->curexpr->childs)
    {
      backupexpr = ctx->curexpr;
      ctx->curexpr = ctx->curexpr->childs;
      ret = revm_expr_unlinkrec(ctx, exprfree, datafree);
      ctx->curexpr = backupexpr;
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed unlink recursion", -1);
    }

  ret = revm_expr_erase(ctx);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to erase top-level expression", -1);
		
  /*
  removeme = (((ctx->curexpr->annot && ctx->curexpr->annot->inhash) || exprfree == 0) ? 0 : 1);
  if (removeme)
    {
      if (ctx->curexpr->value)
	revm_destroy_object(ctx->curexpr->value, datafree);
      XFREE(__FILE__, __FUNCTION__, __LINE__, ctx->curexpr);
    }
  */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Destroy an expression and remove it from the hash table */
int		revm_expr_unlink_by_name(char *e, u_char exprfree, u_char datafree)
{
  revmexpr_t	*expr = NULL;
  int		index;
  revmexprctx_t	*ctx;
  int		ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);

  /* Find in which scope it was defined, and unregister it, possibly unshadowing previous variables */
  for (index = world.curjob->curscope; !expr && index >= 0; index--)
    expr = hash_get(&world.curjob->recur[index].exprs, e);

  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression name", -1);

  ctx = revm_expr_context_init(expr, NULL, 0, expr->label);
  if (!ctx)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to allocate context", -1);
  ret = revm_expr_unlink(ctx, exprfree, datafree);
  revm_expr_context_destroy(ctx);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to unlink expression by name", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/** Destroy an expression and remove it from the hash table : front end function */
int		revm_expr_destroy_by_name(char *ename)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ename || *ename != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  ret = revm_expr_unlink_by_name(ename, 1, 1);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to destroy expression by name", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Destroy an expression and remove it from the hash table : front end function */
int		revm_expr_destroy(revmexpr_t *expr)
{
  revmexprctx_t	*ctx;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  ctx = revm_expr_context_init(expr, NULL, 0, expr->label);
  if (!ctx)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to allocate context", -1);
  ret = revm_expr_unlink(ctx, 1, 1);
  revm_expr_context_destroy(ctx);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to destroy expression", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Remove expression from the hash table without destruction : front end function */
int		revm_expr_hide(char *ename)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ename || *ename != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  ret = revm_expr_unlink_by_name(ename, 0, 0);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to hide expression", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** Remove expression from the hash table without data destruction : front end function */
int		revm_expr_clean(char *ename)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!ename || *ename != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  ret = revm_expr_unlink_by_name(ename, 1, 0);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to clean expression", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* This function lookup an expression from an object id */
revmexpr_t	*revm_expr_lookup(u_int oid)
{
  revmexpr_t	*expr;
  container_t	*cont;
  char		logbuf[BUFSIZ];
  aspectype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (oid >= world.mjr_session.cur->next_id)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid id for container", NULL);  
  cont = world.mjr_session.cur->reg_containers[oid];
  type = aspect_type_get_by_id(cont->type);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid type for container", NULL);  
  snprintf(logbuf, sizeof(logbuf), "%c%s_"AFMT, 
	   REVM_VAR_PREFIX, type->name, *(eresi_Addr *) cont->data);
  expr = revm_expr_get(logbuf);
  if (!expr)
    {
      fprintf(stderr, "Name Expr %s \n", logbuf);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Invalid name for expression", NULL);  
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}
