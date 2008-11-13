/*
** @file arith.c
** @ingroup librevm_lang
** @brief Implementation of arithmetics for expressions
**
** Started on Nov 11 2008 23:39:51 jfv
** $Id
*/
#include "revm.h"


/** Get an expression by its name */
static revmexpr_t	*revm_ename_get(char **str)
{
  char			name[BUFSIZ];
  revmexpr_t		*res;
  u_int			idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(name, BUFSIZ);
  if (**str != '$')
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid expression name", NULL);
  *name = **str;
  (*str)++;
  idx = 1;
  while (**str)
    {
      switch (**str)
	{
	case REVM_VAR_PREFIX:
	case ',':
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid expression name", NULL);
	case '+':
	case '-':
	case '/':
	case '%':
	case '*':
	case '(':
	case ')':
	  (*str)--;
	  goto end;
	default:
	  name[idx++] = **str;
	}
      (*str)++;
    }

 end:
  if (!**str)
    (*str)--;
  res = revm_expr_get(name);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);
}


/** Compute an intermediate numerical result */
static revmexpr_t	*revm_compute_intermediate(revmexpr_t **left, revmexpr_t **right,
						   revmexpr_t *res, u_char *op)
{
  u_char		quiet;
  int			ret;
  revmexpr_t		*final;
  revmobj_t		*ofinal;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!*left)
    {
      *left = res;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);
    }
  if (*right)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Missing arithmetics in expression", NULL);
  *right = res;
  if (*op == REVM_OP_UNKNOW)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No operation to do ?", NULL);
  quiet = world.state.revm_quiet;
  world.state.revm_quiet = 1;
  ofinal = revm_create_IMMED((*left)->type->type, 1, 0);
  final = revm_expr_create_from_object(ofinal, revm_tmpvar_create(), 0);
  ret = revm_arithmetics(final, *left, *right, *op);
  world.state.revm_quiet = quiet;
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to compute intermediate result", NULL);
  *right = NULL;
  *left = final;
  *op = REVM_OP_UNKNOW;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, final);
}


/** Recursive eager evaluation of arithmetic expressions in ERESI */
static revmexpr_t	*revm_compute_rec(char **str)
{
  revmexpr_t		*res;
  revmexpr_t		*left;
  revmexpr_t		*right;
  u_char		op;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  op = REVM_OP_UNKNOW;
  for (left = right = res = NULL; **str; (*str)++)
    switch (**str)
      {

      case '(':
	++(*str);
	res = revm_compute_rec(str);
	if (!res)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid subexpression : remove spaces ?", NULL);
	res = revm_compute_intermediate(&left, &right, res, &op);
	if (!res)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic", NULL);
	break;

      case ')':
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);

      case REVM_VAR_PREFIX:
	res = revm_ename_get(str);
	if (!res)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Unknown expression name", NULL);
	res = revm_compute_intermediate(&left, &right, res, &op);
	if (!res)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic", NULL);
	break;

	/* Operation ! */
      case '+':
	if (op != REVM_OP_UNKNOW || !left)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic syntax", NULL);
	op = REVM_OP_ADD;
	break;
      case '-':
	if (op != REVM_OP_UNKNOW || !left)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic syntax", NULL);
	op = REVM_OP_SUB;
	break;
      case '*':
	if (op != REVM_OP_UNKNOW || !left)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic syntax", NULL);
	op = REVM_OP_MUL;
	break;
      case '/':
	if (op != REVM_OP_UNKNOW || !left)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic syntax", NULL);
	op = REVM_OP_DIV;
	break;
      case '%':
	if (op != REVM_OP_UNKNOW || !left)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid arithmetic syntax", NULL);
	op = REVM_OP_MOD;
	break;

	/* Other characters are invalid in numerical expressions */
      default:
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid expression : remove spaces ?", NULL);
      }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);
}



/** Eager evaluation of arithmetic expressions in ERESI */
revmexpr_t	*revm_compute(char *str)
{
  int		open;
  int		close;
  char		*back;
  revmexpr_t	*res;
  u_char	last[BUFSIZ];
  u_int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  bzero(last, BUFSIZ);
  idx = open = close = 0;
  for (back = str; *back; back++)
    switch (*back)
      {
      case '(':
	if (idx >= BUFSIZ - 1)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Too big expression", NULL);
	last[idx++] = 1;
	open++;
	break;
      case ')':
	if (!idx || last[idx - 1] != 1)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid closing parenthesis", NULL);
	last[idx--] = 0;				
	close++;
	break;
      default:
	break;
      }
  if (open != close)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parenthesing in expression : remove spaces ?", NULL);
  res = revm_compute_rec(&str);
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid numerical expression : remove spaces ?", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);
}
