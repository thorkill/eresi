/**
 * @file op_stosd.c
 * @ingroup handlers_ia32
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_stosd" opcode="0xab"/>
*/

int op_stosd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_STOSD;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_STORE;

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_YDEST, new);

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new);
  new->op[1].type = ASM_OPTYPE_REG;
  new->op[1].len = 0;
  new->op[1].regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op[1].baser = ASM_REG_EAX;

  return (new->len);
}
