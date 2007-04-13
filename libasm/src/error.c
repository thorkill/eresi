/*
** 
** $Id: error.c,v 1.3 2007-04-13 06:56:34 heroine Exp $
** 
** Author  : <kahmm@altdev.net>
** Started : Sun Nov 30 19:58:38 2003
** Updated : Thu Dec  4 01:18:48 2003
*/

#include <libasm.h>


/**
 * Set error
 * @param ins Pointer to instruction structure.
 * @param err Error number
 * @param msg Error message
 */
void	asm_set_error(asm_instr *ins, int err, char *msg)
{
  // ins->proc->error_code = err;
}


/**
 * Set error message
 * @param ins Pointer to instruction structure
 * @param msg Error message
 */

void	asm_set_errormsg(asm_instr *ins, char *msg)
{


}




/**
 * Return error code.
 * @param ins Pointer to instruction structure.
 * @return Return error code.
 */

int	asm_get_error(asm_instr *ins)
{
  return (0);
}

/**
 * Return error message.
 * @param ins Pointer to instruction structure.
 * @return return error message
 */

const char	*asm_get_errormsg(asm_instr *ins)
{
  return ("no error implemented");
}
