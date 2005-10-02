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
  int		ret;
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = vm_move_pc(world.curjob->curcmd->param[0]);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

int		cmd_je()
{
  elfshpath_t	*res;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (!res->immed_val.ent)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jne()
{
  elfshpath_t	*res;
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jg()
{
  elfshpath_t	*res;
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent > 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jl()
{
 elfshpath_t	*res;
  int		ret;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 

 res = hash_get(&vars_hash, "_");
 if (!res)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot retreive last result variable", -1);
 if (res->immed_val.ent < 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
   
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jge()
{
  elfshpath_t	*res;
  int		ret;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent >= 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_jle()
{
  elfshpath_t	*res;
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  res = hash_get(&vars_hash, "_");
  if (!res)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot retreive last result variable", -1);
  if (res->immed_val.ent <= 0)
    {
      ret = vm_move_pc(world.curjob->curcmd->param[0]);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
    }
    
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
