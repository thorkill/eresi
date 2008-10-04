/**
 * @file op_xor_rmb_rb.c
 * @ingroup handlers_ia32
 * @brief Handler for instruction xor rmb,rb opcode 0x30
 * $Id: op_xor_rmb_rb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction xor rmb,rb opcode 0x30
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction disassembled.
*/

int op_xor_rmb_rb(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XOR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE,
                                new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERALBYTE,
                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE,
                                new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERALBYTE,
                                new);
#endif
  return (new->len);
}
