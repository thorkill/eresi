#include <libasm.h>
#include <libasm-int.h>


/**
 * Main function, dispatch processing to handler.
 * @param op Pointer to operand to fill
 * @param opcode Pointer to operand data.
 * @param otype Content of operand to fetch : ASM_OTYPE_*
 * @param proc Pointer to processor structure.
 * @return Operand length or -1 on error (should currently never occur)
 */
int asm_sparc_op_fetch(asm_operand *op, unsigned char *opcode, int otype, 
		       asm_instr *ins)
{ 
  vector_t      *vec;
  unsigned int         dim[1];
  int           to_ret;
  int           (*fetch)(asm_operand *, unsigned char *, int, asm_instr *);
  
  vec = aspect_vector_get("operand-sparc");
  dim[0] = otype;
  
  fetch = aspect_vectors_select(vec, dim);
  to_ret = fetch(op, opcode, otype, ins);

  if (to_ret == -1)
  {
    printf("%s:%i Unsupported operand content : %i\n", __FILE__, __LINE__, 
    	     otype);
  }

  op->name = asm_sparc_get_op_name(op);

  return (to_ret);
}
