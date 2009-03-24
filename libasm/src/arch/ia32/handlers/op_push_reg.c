/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_reg" opcode="0x50"/>
  <instruction func="op_push_reg" opcode="0x51"/>
  <instruction func="op_push_reg" opcode="0x52"/>
  <instruction func="op_push_reg" opcode="0x53"/>
  <instruction func="op_push_reg" opcode="0x54"/>
  <instruction func="op_push_reg" opcode="0x55"/>
  <instruction func="op_push_reg" opcode="0x56"/>
  <instruction func="op_push_reg" opcode="0x57"/>
*/

int op_push_reg(asm_instr *new, u_char *opcode, u_int len, 
                asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_PUSH;
  new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
  new->spdiff = -4;

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_OPMOD, new);

  if (new->op[0].baser == ASM_REG_EBP)
    new->type |= ASM_TYPE_PROLOG;

  return (new->len);
}
