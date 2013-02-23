/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction bsr rv, rms, opcode 0x0f 0xbd
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int i386_bsr_rv_rmb(asm_instr *new, u_char *opcode, u_int len,
		    asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_BSR;

  new->type = ASM_TYPE_BITTEST | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);

  return (new->len);
}
