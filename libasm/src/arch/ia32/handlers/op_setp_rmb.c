/**
 * @file op_setp_rmb.c
 * $Id: op_setp_rmb.c,v 1.3 2007-05-19 23:59:12 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction setp, opcode 0x0f 0x9a
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_setp_rmb(asm_instr *new, u_char *opcode, u_int len, 
		asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_SET_PARITY;
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmb(&new->op1, opcode + 1, len - 1, proc);
  new->len += new->op1.len + 1;
#endif
  return (new->len);
}
