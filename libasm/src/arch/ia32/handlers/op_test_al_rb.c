/**
 * @file op_test_al_rb.c
 * @ingroup handlers_ia32
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction test al,rb opcode 0xa8
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_test_al_rb(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  new->instr = ASM_TEST;
  new->len += 1;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                          ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
					       ASM_REGSET_R8));

  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].baser = ASM_REG_AL;
  new->op[0].regset = ASM_REGSET_R8;
  new->len += asm_operand_fetch(&new->op[1], opcode + 1,
                                ASM_CONTENT_IMMEDIATEBYTE, new);
#endif

  return (new->len);
}
