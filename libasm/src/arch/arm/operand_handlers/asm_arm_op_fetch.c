/**
* @file libasm/src/arch/arm/operand_handlers/asm_arm_op_fetch.c
** @ingroup ARM_operands
*/
#include <libasm.h>


/**
 * Main function, dispatch processing to handler.
 * @param op Pointer to operand to fill
 * @param opcode Pointer to operand data.
 * @param otype Content of operand to fetch : ASM_OTYPE_*
 * @param proc Pointer to processor structure.
 * @return Operand length or -1 on error (should currently never occur)
 */
int asm_arm_op_fetch(asm_operand *op, u_char *opcode, int otype, 
		       asm_instr *ins)
{ 
  vector_t      *vec;
  u_int         dim[1];
  int           to_ret;
  int           (*fetch)(asm_operand *, u_char *, int, asm_instr *);

  LIBASM_PROFILE_FIN();  

  vec = aspect_vector_get(LIBASM_VECTOR_OPERAND_ARM);
  dim[0] = otype;
  
  fetch = aspect_vectors_select(vec, dim);
  to_ret = fetch(op, opcode, otype, ins);

  if (to_ret == -1)
  {
    printf("%s:%i Unsupported operand content : %i\n", __FILE__, __LINE__, 
    	     otype);
  }

  op->name = asm_arm_get_op_name(op);

  LIBASM_PROFILE_FOUT(to_ret);
}
