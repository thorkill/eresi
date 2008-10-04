/*
** $Id: op_in_al_dx.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_al_dx" opcode="0xec"/>
 */

int     op_in_al_dx(asm_instr *new, unsigned char *opcode, unsigned int len,
                         asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
					       ASM_REGSET_R8));
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
#endif

  new->ptr_instr = opcode;
  new->op[0].content = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_AL;

#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new);
#endif

  new->op[1].content = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[1].regset = ASM_REGSET_R16;
  new->op[1].baser = ASM_REG_DX;

  return (new->len);
}
