/**
 * @file op_xor_rv_rmv.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xor, opcode 0x33
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor, opcode 0x33
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_xor_rv_rmv(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  new->instr = ASM_XOR;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_GENERAL, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);

  return (new->len);
}
