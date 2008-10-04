/**
 * @file op_stosd.c
 * @ingroup handlers_ia32
** $Id: op_stosd.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_stosd" opcode="0xab"/>
*/

int op_stosd(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) 
{
  new->instr = ASM_STOSD;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_STORE;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new, 0);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new);
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_FIXED, new);
  new->op[1].content = ASM_OP_BASE;
  new->op[1].len = 0;
  new->op[1].regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op[1].baser = ASM_REG_EAX;
#endif

  return (new->len);
}
