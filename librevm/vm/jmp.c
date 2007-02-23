/*
** jmp.c for  in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Thu Nov 20 05:09:43 2003 mayhem
** Last update Thu Nov 20 05:10:03 2003 mayhem
*/
#include "revm.h"



int		cmd_jmp()
{
  int		ret;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = vm_move_pc(world.curjob->curcmd->param[0]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int		cmd_je()
{
  revmobj_t	*res;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (!res->immed_val.ent)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jne()
{
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jg()
{
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent > 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jl()
{
 revmobj_t	*res;
  int		ret;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

 res = hash_get(&vars_hash, "_");
 if (!res)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot retreive last result variable", -1);
 if (res->immed_val.ent < 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
   
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jge()
{
  revmobj_t	*res;
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent >= 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jle()
{
  revmobj_t	*res;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent <= 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
