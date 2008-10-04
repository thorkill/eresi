/**
 * @file op_xchg_rmv_rv.c
 * @brief Handler for instruction xchg rmv,rv opcode 0x87
 * @ingroup handlers_ia32
 * $Id: op_xchg_rmv_rv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xchg rmv,rv opcode 0x87
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_xchg_rmv_rv(asm_instr *instr, unsigned char *opcode, unsigned int len,
                   asm_processor *proc)
{
  instr->len += 1;
  instr->ptr_instr = opcode;
  instr->instr = ASM_XCHG;
  instr->type = ASM_TYPE_LOAD | ASM_TYPE_STORE;

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_OTYPE_ENCODED, 
                                instr, 0);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_OTYPE_GENERAL, 
                                instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_OTYPE_ENCODED, 
                                instr);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_OTYPE_GENERAL, 
                                instr);
#endif
  return (instr->len);
}
