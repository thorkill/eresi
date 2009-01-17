/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_out_ref_ib_al" opcode="0xe6"/>
*/


int op_out_ref_ib_al(asm_instr *new, u_char *opcode, u_int len,
                     asm_processor *proc)
{
  new->instr = ASM_OUT;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_IO | ASM_TYPE_STORE;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_IMMEDIATEBYTE, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_AL,
					       ASM_REGSET_R8));

#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_FIXED, new);
#endif
  new->op[1].type = ASM_OP_BASE;
  new->op[1].regset = ASM_REGSET_R8;
  new->op[1].baser = ASM_REG_AL;

  return (new->len);
}
