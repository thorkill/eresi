/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Main function, dispatch processing to handler.
 * @param op Pointer to operand to fill
 * @param opcode Pointer to operand data.
 * @param otype Type of operand to fetch : ASM_OTYPE_*
 * @param proc Pointer to processor structure.
 * @return Operand length or -1 on error (should currently never occur)
 */

int     asm_operand_fetch(asm_operand *op, u_char *opcode, int otype, 
			  asm_instr *ins)
{ 
  vector_t      *vec;
  u_int         dim[1];
  int           to_ret;
  int           (*fetch)(asm_operand *, u_char *, int, asm_instr *);
  
  vec = aspect_vector_get("operand");
  dim[0] = otype;
  
  fetch = aspect_vectors_select(vec, dim);
  to_ret = fetch(op, opcode, otype, ins);
  if (to_ret == -1)
    {
      printf("%s:%i Unsupported operand type : %i\n", __FILE__, __LINE__, 
	     otype);
    }
  else
    {
      op->sbaser = ((op->content & ASM_OP_BASE) ? 
		    get_reg_intel(op->baser, op->regset) : "");
      op->sindex = ((op->content & ASM_OP_BASE) ? 
		    get_reg_intel(op->indexr, op->regset) : "");
    }
  return (to_ret);
}
