/**
 * @file op_xor_rmv_rv.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xor rmv,rv opcode 0x31
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor rmv,rv opcode 0x31
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble
 * @param len Length of data to disassemble
 * @param proc Pointer to processor structure
 * @return Insruction length
  <instruction func="op_xor_rmv_rv" opcode="0x31"/>
*/

int op_xor_rmv_rv(asm_instr *instr, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  instr->len += 1;
  instr->ptr_instr = opcode;
  instr->instr = ASM_XOR;
  instr->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  instr->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_CONTENT_ENCODED, instr);
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_CONTENT_GENERAL, instr);

  return (instr->len);
}
