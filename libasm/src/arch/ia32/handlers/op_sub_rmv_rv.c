/**
 * @file op_sub_rmv_rv.c
 * @ingroup handlers_ia32
** $Id: op_sub_rmv_rv.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction sub rmv,rv opcode 0x29
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_sub_rmv_rv(asm_instr *new, unsigned char *opcode, unsigned int len, 
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_SUB;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,
                                new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,
                                new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_OTYPE_GENERAL, new);
#endif

  return (new->len);
}
