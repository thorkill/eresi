/**
 * @file op_lodsb.c
 * @ingroup handlers_ia32
 * $Id: op_lodsb.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction func="op_lodsb" opcode="0xac"/>
 */

int op_lodsb(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->instr = ASM_LODSB;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new,
								asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new);
#endif
  new->op[1].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[1].regset = asm_proc_opsize(proc) ?
                      ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op[1].baser = ASM_REG_EAX;

  return (new->len);
}
