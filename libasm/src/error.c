/**
 * @file error.c
 * @ingroup libasm_engine
** 
** $Id$
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
  if (ins && ins->proc)
    ins->proc->error_code = err;
}


/**
 * Set error message.
 * This is currently not implemented.
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
 * @return Return a pointer to a static string containing error message.
 */

const char	*asm_get_errormsg(asm_processor *proc)
{
  if (proc)
    {
      switch(proc->error_code)
	{
	case LIBASM_ERROR_SUCCESS: return (LIBASM_MSG_SUCCESS);
	case LIBASM_ERROR_NSUCH_CONTENT: return (LIBASM_MSG_SUCCESS);
	case LIBASM_ERROR_ILLEGAL: return (LIBASM_MSG_SUCCESS);
	case LIBASM_ERROR_TOOSHORT: return (LIBASM_MSG_TOOSHORT);
	}
      return (LIBASM_MSG_ERRORNOTIMPLEMENTED);
    }
  else
    return ("asm_get_errormsg: proc is NULL");
}
