/**
 * @file op_sub_rv_rmv.c
 * @brief Handler for instruction sub rv,rmv opcode 0x2b
 * @ingroup handlers_ia32
 * 
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction sub rv,rmv opcode 0x2b
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_sub_rv_rmv(asm_instr *instr, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  instr->len += 1;
  instr->instr = ASM_SUB;
  instr->ptr_instr = opcode;
  instr->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  instr->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_GENERAL, instr, 0);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_CONTENT_ENCODED, instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_GENERAL, instr);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_CONTENT_ENCODED, instr);
#endif

  return (instr->len);
}
