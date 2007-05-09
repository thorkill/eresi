/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
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
			  asm_processor *proc)
{ 
  vector_t      *vec;
  u_int         dim[2];
  int           to_ret;
  int           (*fetch)(asm_operand *, u_char *, int, asm_processor *);
  
  vec = aspect_vector_get("operand");
  dim[0] = LIBASM_VECTOR_IA32;
  dim[1] = otype;
  
  fetch = aspect_vectors_select(vec, dim);
  to_ret = fetch(op, opcode, otype, proc);
  if (to_ret == -1)
    {
      printf("%s:%i Unsupported operand type : %i\n", __FILE__, __LINE__, 
	     otype);
    }
  return (to_ret);
}
