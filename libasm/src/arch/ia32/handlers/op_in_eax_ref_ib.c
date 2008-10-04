/*
** $Id: op_in_eax_ref_ib.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_ref_ib" opcode="0xe5"/>
*/

int op_in_eax_ref_ib(asm_instr *new, unsigned char *opcode, unsigned int len,
		     asm_processor *proc)
{
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new, 
								asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
  new->op[0].content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATEBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATEBYTE, new);
#endif
  new->op[1].content = ASM_OP_REFERENCE | ASM_OP_VALUE;

  return (new->len);
}
