/*
** create.c : Implementation of scripting declarations for meta-language variables
**
** Started Jan 23 2007 23:39:51 mayhem
**
** $Id: expressions.c,v 1.1 2007-07-07 17:30:24 may Exp $
**
*/
#include "revm.h"



/* Get the real value of the parameter string for further revmobj initialization */
revmexpr_t	*revm_expr_read(char **datavalue)
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
  
  /* First get the field (or root type) label */
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

	    //DEBUG
	    //fprintf(stderr, "NEW REVMEXPR = %s :: %s \n", 
	    //    expr->label, expr->strval);
	    
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
      
      //DEBUG
      //fprintf(stderr, "NEW REVMEXPR = %s ::: %s \n", 
      //expr->label, expr->strval);
      
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
    }
  
  /* Default error */
  XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Malformed parent field value", NULL);
}




/* Initialize a (potentially record) ERESI variable */
revmexpr_t	*revm_expr_set(char		*curpath, 
			       revmexpr_t	*curexpr,
			       aspectype_t	*curtype, 
			       void		*srcdata,
			       char		*datavalue)
{
  static u_int	pathsize = 0;
  char		pathbuf[BUFSIZ + 1] = {0x00};
  char		*recpath;
  revmexpr_t	*newexpr, *childexpr, *next;
  void		*childata;
  aspectype_t	*childtype;
  revmobj_t	*curdata;
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary processing */
  if (*curpath == '$')
    {
      strncpy(pathbuf, curpath + 1, BUFSIZ);
      recpath = pathbuf;
      pathsize = strlen(curpath + 1);
    }
  else
    recpath = curpath;

  // DEBUG
  //fprintf(stderr, "Current expr path(%s) \n", recpath);

  /* Construct the expression until end of ascii string */
  while (*datavalue)
    {
      
      // DEBUG
      //fprintf(stderr, "Current datavalue = (%s) \n", datavalue);

      /* Read next typed expression in string */
      newexpr    = revm_expr_read(&datavalue);

      /* If we are at the root expression, type might have been implicit */
      if (newexpr->label == NULL)
	{
	  newexpr->label = curtype->name;
	  if (!curexpr)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid field name for expression", NULL);
	  curexpr = newexpr;
	  datavalue = newexpr->strval;
	  continue;
	}

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

      /* Non-terminal case : we will need to recurse */
      if (childtype->childs)
	{
	  //DEBUG
	  //fprintf(stderr, "NOW RECORD field, recursing ! \n");
	  
	  childata = (char *) srcdata + childtype->off;
	  len = snprintf(pathbuf + pathsize, BUFSIZ - pathsize, 
			 ".%s", childtype->fieldname);
	  vm_inform_type_addr(childtype->name, recpath, 
			      (elfsh_Addr) childata, 0);
	  pathsize += len;

	  /* Insert child where necessary */ 
	  childexpr = revm_expr_set(recpath, newexpr, childtype, 
				    childata, newexpr->strval);
	  if (!newexpr->childs)
	    newexpr->childs = childexpr;
	  else
	    {
	      for (next = newexpr->childs; next->next; next = next->next);
	      next->next = childexpr;
	    }

	  pathsize -= len;
	  bzero(pathbuf + pathsize, len);
	}

      /* Terminal case : no recursion */
      else
	{
	  //DEBUG
	  //fprintf(stderr, "NOW Terminal field, setting its value\n");
	  
	  /* Handle RAW terminal field */
	  if (curtype->type == ASPECT_TYPE_RAW)
	    {
	      //XXX: Call hexa converter curval.datastr and set field
	      fprintf(stderr, " [E] Raw object initialization unsupported\n");
	      continue;
	    }

	  /* Lookup scalar value and assign it to the field */
	  newexpr->value = vm_revmobj_lookup_real(curtype, recpath, 
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
      if (!curexpr)
	curexpr = newexpr;
      else
	{
	  for (next = curexpr; next->next; next = next->next);
	  next->next = newexpr;
	}

    }

  /* Return success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, curexpr);
}




/* Print a specific type */
/*
int		revm_expr_print(revmexpr_t *curexpr, void *data)
{
  aspectype_t	*child;
  char		pathbuf[BUFSIZ];

  // Just for printing 
  char		buf[BUFSIZ];
  char		prefix[128];


  int		len;
  char		*size;
  char		offset[128];
  int		idx;
  int		sz;
  char		*pad;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  // Preliminary checks
  typename = curtype->name;
  
  // Utiliser pathbuf comme string courante "str" pour vm_field_get !
  //
  //vm_field_get(type, str, (void **) &data);
  //
  //sur la variable revmannot_t (ou le revmobj_t ?) informee par cmd_declare
  //
  // et ce pour tout les fields du type
  //
  // XXX ALSO NEED TO DECLARE $typename_key ON VARIABLE CREATION ! 
  // Set up things for printing
  if (op == REVM_OBJECT_PRINT)
  {
      vm_endline();
      snprintf(prefix, sizeof(prefix), "%s%s%s", 
	       vm_colorfieldstr("{"),
	       vm_colornumber("%u", curtype->size),
	       vm_colorfieldstr("}"));
      len = snprintf(buf, sizeof(buf), "  %s %-20s %s %-10s", 
		     vm_colorfieldstr(typename),
		     vm_colortypestr_fmt("%-20s", typevar), 
		     vm_colorfieldstr("size"),
		     prefix);
      size = alloca(20);
    }


    // If the type is a structure
    if (curtype->childs)
    {

      // Prepare the padding after each field name
      // -14 is dirty: some bug in colors 
      if (op == REVM_OBJECT_PRINT)
	{
	  len += snprintf(buf + len, sizeof(buf) - len, "%s",
			  vm_colorfieldstr(" = {"));
	  sz = len - vm_color_size(buf) - 14; 
	  pad = alloca(sz + 1);
	  memset(pad, ' ', sz);
	  pad[sz] = 0x00;
	}


      // For each child field type
      for (child = curtype->childs; child; child = child->next)
	{
	  
	  // Compute the size field 
	  if (child->type == ASPECT_TYPE_RAW)
	    snprintf(size, sizeof(size), "%s%s%s", 
		     vm_colorfieldstr("["), 
		     vm_colornumber("%u", child->size), 
		     vm_colorfieldstr("]"));
	  else if (child->dimnbr && child->elemnbr)
	    {
	      for (sz = idx = 0; idx < child->dimnbr; idx++)
		sz += 20;
	      size = alloca(sz);
	      for (sz = idx = 0; idx < child->dimnbr; idx++)
		sz += sprintf(size + sz, "%s%s%s", 
			      vm_colorfieldstr("["),
			      vm_colornumber("%u", child->elemnbr[idx]),
			      vm_colorfieldstr("]"));
	    }
	  else
	    *size = 0x00;

	  // Format the whole thing 
	  snprintf(offset, sizeof(offset), "%s%s", 
		   vm_colornumber("%u", child->off),
		   vm_colorfieldstr(":"));
	  len += snprintf(buf + len, sizeof(buf) - len, 
			  "%s%s%s%s%s%s%s", 
			  offset, 
			  vm_colorstr(child->fieldname),
			  vm_colorfieldstr(":"),
			  vm_colortypestr((child->isptr ? "*" : "")), 
			  vm_colortypestr(child->name), 
			  size,
			  vm_colorfieldstr((child->next ? ",\n" : "}\n\n")));
	  
	  // Print field and next padding 
	  vm_output(buf);
	  vm_endline();
	  if (child->next)
	    vm_output(pad);
	  *buf = 0x00;
	  len = 0;
	}
    }

    // Print non-structures types 
    else
    {
    vm_output(buf);
    vm_output("\n");
    }

// Return success 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}
*/







/* Get an expression giving a variable name */
/*
revmexpr_t	*revm_expr_get(char *pathname)
{


}
*/


/* Compare 2 typed expressions */
/*
int		revm_expr_compare(revmexpr_t *source, void *sdata,
				  revmexpr_t *candid, void *cdata)
{

}
*/
