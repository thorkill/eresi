/**
 * @defgroup m-operands MIPS operands API.
 * @ingroup mips
 */
/**
** @file asm_mips_operand_fetch.c
** @ingroup m-operands
*/
/*
 * Started by Adam 'pi3' Zabrocki
 * $Id$
 */

#include <libasm.h>

int asm_mips_operand_fetch(asm_operand *op, u_char *opcode, int otype,
                                          asm_instr *ins)

{ 
  vector_t      *vec;
  u_int         dim[1];
  int           ret;
  int           (*func_op)(asm_operand *op, u_char *opcode, int otype, asm_instr *ins);
  
  vec = aspect_vector_get(LIBASM_VECTOR_OPERAND_MIPS);
  dim[0] = otype;
  
  func_op = aspect_vectors_select(vec, dim);
  if ( (ret = func_op(op, opcode, otype, ins)) == -1) {
    printf("%s:%i Unsupported operand content : %i\n", __FILE__, __LINE__, 
    	     otype);
  }

  return (ret);
}
