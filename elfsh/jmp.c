/*
** jmp.c for  in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Thu Nov 20 05:09:43 2003 mayhem
** Last update Thu Nov 20 05:10:03 2003 mayhem
*/
#include "elfsh.h"



int		cmd_jmp()
{

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  return (vm_move_pc(world.curjob->curcmd->param[0]));
}

int		cmd_je()
{
  elfshpath_t	*res;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
  if (!res->immed_val.ent)
    return (vm_move_pc(world.curjob->curcmd->param[0]));
  return (0);
}

int		cmd_jne()
{
  elfshpath_t	*res;
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
  if (res->immed_val.ent)
    return (vm_move_pc(world.curjob->curcmd->param[0]));
  return (0);
}

int		cmd_jg()
{
  elfshpath_t	*res;
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
  if (res->immed_val.ent > 0)
    return (vm_move_pc(world.curjob->curcmd->param[0]));
  return (0);
}

int		cmd_jl()
{
 elfshpath_t	*res;

 E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 

 res = hash_get(&vars_hash, "_");
 if (!res)
   ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
 if (res->immed_val.ent < 0)
   return (vm_move_pc(world.curjob->curcmd->param[0]));
 return (0);
}

int		cmd_jge()
{
  elfshpath_t	*res;
  
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
  if (res->immed_val.ent >= 0)
    return (vm_move_pc(world.curjob->curcmd->param[0]));
  return (0);
}

int		cmd_jle()
{
  elfshpath_t	*res;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_SETERROR("[elfsh:cmd_jmp] Cannot retreive last result variable\n", -1);
  if (res->immed_val.ent <= 0)
    return (vm_move_pc(world.curjob->curcmd->param[0]));
  return (0);
}
