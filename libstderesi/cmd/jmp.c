/*
** @file jmp.c
** 
** Made by jfv
** Login   <jfv@devhell.org>
** 
** Started on  Thu Nov 20 05:09:43 2003 jfv
** Last update Thu Nov 20 05:10:03 2003 jfv
**
** $Id: jmp.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"



int		cmd_jmp()
{
  int		ret;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = revm_move_pc(world.curjob->curcmd->param[0]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int		cmd_je()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  res = last->value;
  if (!res->immed_val.ent)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jne()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  res = last->value;
  if (res->immed_val.ent)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jg()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  res = last->value;
  if (res->immed_val.ent > 0)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jl()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

 last = revm_expr_get(REVM_VAR_RESULT);
 if (!last || !last->value)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot retreive last result variable", -1);
  res = last->value;
 if (res->immed_val.ent < 0)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
   
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jge()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  res = last->value;
  if (res->immed_val.ent >= 0)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jle()
{
  revmexpr_t	*last;
  revmobj_t	*res;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  last = revm_expr_get(REVM_VAR_RESULT);
  if (!last || !last->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  res = last->value;
  if (res->immed_val.ent <= 0)
    {
      ret = revm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
