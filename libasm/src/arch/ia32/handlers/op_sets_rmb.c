/**
 * @file op_sets_rmb.c
 * @ingroup handlers_ia32
 * $Id$
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction sets, opcode 0x0f 0x98
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int op_sets_rmb(asm_instr *new, u_char *opcode, u_int len, 
		asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_SET_SIGNED;
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE,
				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODEDBYTE,
				new);
#endif
  return (new->len);
}
