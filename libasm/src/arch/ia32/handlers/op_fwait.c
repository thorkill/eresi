/** 
 * $Id$
* @file libasm/src/arch/ia32/handlers/op_fwait.c
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Instruction handler for opcode 0x9b.
 * This opcode is the for fwait prefix opcode.
 * Disassembling is forwarded on next byte after execution of this
 * handler.
 * 
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to buffer to disassemble.
 * @param len Length of buffer to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of disassembled instruction.
 */

int op_fwait(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  if (!new->ptr_prefix)
    new->ptr_instr = opcode;
  new->prefix |= ASM_PREFIX_FWAIT;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
