/**
 * @file op_test_eax_iv.c
 * @ingroup handlers_ia32
 * $Id$
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction test eax,iv opcode 0xa9
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
 */

int op_test_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  new->instr = ASM_TEST;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                          ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_IMMEDIATE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op[0].baser = ASM_REG_AX;
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_IMMEDIATE, new);
#endif

  return (new->len);
}
