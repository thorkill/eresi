/**
 * @file op_seto_rmb.c
 * @ingroup handlers_ia32
 * $Id: op_seto_rmb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction seto, opcode 0x0f 0x90
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction
*/

int op_seto_rmb(asm_instr *new, unsigned char *opcode, unsigned int len, 
		asm_processor *proc) 
{
  new->len += 1;
  new->instr = ASM_SET_OVERFLOW;
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new);
#endif 
 return (new->len);
}
