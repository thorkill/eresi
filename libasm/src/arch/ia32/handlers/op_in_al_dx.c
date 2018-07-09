/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_in_al_dx" opcode="0xec"/>
 */

int     op_in_al_dx(asm_instr *new, u_char *opcode, u_int len,
                         asm_processor *proc) {
  new->len += 1;
  new->instr = ASM_IN;
  new->type = ASM_TYPE_LOAD | ASM_TYPE_IO;

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);

  new->ptr_instr = opcode;
  new->op[0].type = ASM_OPTYPE_REG;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_AL;

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED, new);

  new->op[1].type = ASM_OPTYPE_MEM;
  new->op[1].memtype = ASM_OP_BASE | ASM_OP_REFERENCE;
  new->op[1].regset = ASM_REGSET_R16;
  new->op[1].baser = ASM_REG_DX;

  return (new->len);
}
