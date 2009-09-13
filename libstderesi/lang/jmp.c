/*
* @file libstderesi/lang/jmp.c
** @ingroup lang 
** Made by jfv
** Login   <jfv@devhell.org>
** 
** Started on  Thu Nov 20 05:09:43 2003 jfv
** Last update Thu Nov 20 05:10:03 2003 jfv
**
** $Id$
**
*/
#include "libstderesi.h"


/**
 * Handler for the jmp instruction
 */ 
int		cmd_jmp()
{
  int		ret;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = revm_move_pc(world.curjob->curcmd->param[0]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

/**
 * Handler for the je instruction
 */ 
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

/**
 * Handler for the jne instruction
 */ 

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

/**
 * Handler for the jg instruction
 */ 

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

/**
 * Handler for the jl instruction
 */ 
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


/**
 * Handler for the jge instruction
 */ 
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

/**
 * Handler for the jle instruction
 */ 
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
