/*
** 
** error.c in 
** 
** Started : Sun Nov 30 19:58:38 2003
** Updated : Thu Dec  4 01:18:48 2003
*/

#include <libasm.h>

void	asm_set_error(asm_instr *ins, int err)
{
  // ins->proc->error_code = err;
}



void	asm_set_errormsg(asm_instr *ins, char *msg)
{


}





int	asm_get_error(asm_instr *ins)
{

  return (0);


}


const char	*asm_get_errormsg(asm_instr *ins)
{

  return ("no error implemented");
}
