/**
 * @file op_setg_rmb.c
 * @ingroup handlers_ia32
 * $Id: op_setg_rmb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction setg, opcode 0x0f 0x9f
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int op_setg_rmb(asm_instr *new, unsigned char *opcode, unsigned int len, 
		asm_processor *proc)
{
  new->instr = ASM_SET_S_GREATER;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new);
#endif

  return (new->len);
}
