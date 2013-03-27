/**
* @file libstderesi/cmd/print.c
** @ingroup cmd
** Started on  Sat Jan 25 11:19:53 2003 jfv
** 
**
** $Id$
**
*/
#include "libstderesi.h"


/** 
 * Print an object 
 * @param obj
 */
void		revm_object_print(revmobj_t *obj)
{
  u_char	byte;
  uint16_t	half;
  uint32_t	word;
  eresi_Addr   	val;
  char		*str;
  char		logbuf[BUFSIZ] = {0x00};
  u_int		idx;
  aspectype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(logbuf, BUFSIZ);
  type = obj->otype;
  if (type && type->isptr)
    goto isptr;

  /* Print a non-pointer type */
  if (!obj->otype)
    snprintf(logbuf, BUFSIZ - 1, "UNKNOW_OBJECT_TYPE_0");
  else
    switch (obj->otype->type)
      {
      case ASPECT_TYPE_BYTE:
	byte = (obj->immed ? obj->immed_val.byte : obj->get_obj(obj->parent));
	word = (uint32_t) byte;
	snprintf(logbuf, BUFSIZ - 1, "'%s'(%s) ", 
		 ((word > 31 && word < 127) ? revm_colornumber("%c", word) : 
		  revm_colorfieldstr("?")), revm_colornumber("%hhd", word));
	break;
      case ASPECT_TYPE_SHORT:
	half = (obj->immed ? obj->immed_val.half : obj->get_obj(obj->parent));
	word = (uint32_t) half;
	snprintf(logbuf, BUFSIZ - 1, "%s(%s) ", 
		 revm_colornumber("%hd", word), revm_colornumber("0x%hX", word));
	break;
      case ASPECT_TYPE_LONG:
      case ASPECT_TYPE_CADDR:
      case ASPECT_TYPE_DADDR:
	val = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
	snprintf(logbuf, BUFSIZ - 1, "%s ", 
		 revm_colornumber(RXFMT, val));
	break;
      case ASPECT_TYPE_INT:
      case ASPECT_TYPE_OID:
	word = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
	snprintf(logbuf, BUFSIZ - 1, "%s ", 
		 revm_colornumber("%d", word));
	break;
      case ASPECT_TYPE_STR:
	str = (obj->immed ? obj->immed_val.str : 
	       obj->get_name(obj->root, obj->parent));
	snprintf(logbuf, BUFSIZ, "%s ", revm_colorfieldstr(str));
	break;
	
	/* XXX: need a config variable to tell how many bytes we want */
      case ASPECT_TYPE_RAW:
	str = (char *) (obj->immed ? obj->immed_val.str : 
			obj->get_name ? obj->get_name(obj->root, obj->parent) : "");
	for (idx = 0; idx < 10 && str[idx]; idx++)
	  snprintf(logbuf, BUFSIZ - 1, "%02X%s", 
		   (u_char) str[idx], (idx < 9 && str[idx + 1] ? " " : ""));
	break;
	
	/* When the object type is unknown */
      default:
	snprintf(logbuf, BUFSIZ - 1, "UNKNOW_OBJECT_TYPE_%u ", obj->otype->type);
	break;
      }
  revm_output(logbuf);
  revm_endline();
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  /* Now print a pointer */
 isptr:
  if (!obj->otype)
    snprintf(logbuf, BUFSIZ - 1, "UNKNOW_PTR_OBJECT_TYPE_0");
  else
    switch (obj->otype->type)
      {
      case ASPECT_TYPE_STR:
      case ASPECT_TYPE_BYTE:
      case ASPECT_TYPE_SHORT:
      case ASPECT_TYPE_LONG:
      case ASPECT_TYPE_CADDR:
      case ASPECT_TYPE_DADDR:
      case ASPECT_TYPE_INT:
      case ASPECT_TYPE_RAW:
      default:
	val = (obj->immed || !obj->parent ? obj->immed_val.ent : obj->get_obj(obj->parent));
	snprintf(logbuf, BUFSIZ - 1, "%s ", revm_colornumber(RXFMT, val));
	break;
    }
  revm_output(logbuf);
  revm_endline();
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/**
 * Print a string 
 */
int			cmd_print()
{
  revmexpr_t		*expr;
  volatile u_int	idx;
  char			logbuf[BUFSIZ] = {0x00};

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; world.curjob->curcmd->param[idx] != NULL; idx++)
    {
      expr = revm_expr_get(world.curjob->curcmd->param[idx]);
      if (!expr)
	expr = revm_compute(world.curjob->curcmd->param[idx]);
      if (expr && expr->value && !expr->value->perm)
	{
	  revm_object_print(expr->value);
	  revm_expr_destroy_by_name(expr->label);
	  continue;
	}
      else if (expr)
	{
	  if (*expr->label == REVM_VAR_PREFIX)
	    revm_expr_print_by_name(expr->label, 0);
	  else
	    revm_expr_print_by_name(world.curjob->curcmd->param[idx], 0);
	  revm_output("\n\n");
	  continue;
	}
      expr = revm_lookup_param(world.curjob->curcmd->param[idx], 1);
      if (expr && expr->value && !expr->value->perm)
	{
	  revm_object_print(expr->value);
	  revm_expr_destroy_by_name(expr->label);
	  continue;
	}
      else if (!expr)
	{
	  snprintf(logbuf, BUFSIZ - 1, "%s ", world.curjob->curcmd->param[idx]);
	  revm_output(logbuf);
	  continue;
	}
      else
	revm_expr_print_by_name(expr->label, 0);
    }

  revm_output("\n");
  if (!world.state.revm_quiet)
    revm_output("\n");
  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
