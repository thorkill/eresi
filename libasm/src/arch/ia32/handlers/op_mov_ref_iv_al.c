/**
 * @file op_mov_ref_iv_al.c
 * @ingroup handlers_ia32
 * $Id: op_mov_ref_iv_al.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction func="op_mov_ref_iv_al" opcode="0xa2"/>
 */

int op_mov_ref_iv_al(asm_instr *new, unsigned char *opcode, unsigned int len,
		     asm_processor *proc)
{
  new->instr = ASM_MOV;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_OFFSET, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_OFFSET, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
					       ASM_REGSET_R8));
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
  new->op[1].type = ASM_OTYPE_FIXED;
  new->op[1].regset = ASM_REGSET_R8;
  new->op[1].content = ASM_OP_BASE;
  new->op[1].baser = ASM_REG_AL;

  return (new->len);
}
