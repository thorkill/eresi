/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction in eax,dx opcode 0xed
 * <instruction func="op_in_eax_dx" opcode="0xed"/>
 */

int op_in_eax_dx(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
#endif
  new->op[0].type = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_R32;
  new->op[0].baser = ASM_REG_EAX;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE | ASM_OP_REFERENCE, ASM_REG_DX,
					       ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new);
#endif
  new->op[1].regset = ASM_REGSET_R16;
  new->op[1].baser = ASM_REG_DX;
  new->op[1].type = ASM_OP_BASE | ASM_OP_REFERENCE;

  return (new->len);
}
