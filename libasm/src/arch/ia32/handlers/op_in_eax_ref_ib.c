/*
** $Id$
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_eax_ref_ib" opcode="0xe5"/>
*/

int op_in_eax_ref_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->instr = ASM_IN;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_FIXED, new);
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_IMMEDIATEBYTE, new);
  new->op[1].type = ASM_OP_REFERENCE | ASM_OP_VALUE;
  return (new->len);
}
