/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_inc_reg" opcode="0x40"/>
  <instruction func="op_inc_reg" opcode="0x41"/>
  <instruction func="op_inc_reg" opcode="0x42"/>
  <instruction func="op_inc_reg" opcode="0x43"/>
  <instruction func="op_inc_reg" opcode="0x44"/>
  <instruction func="op_inc_reg" opcode="0x45"/>
  <instruction func="op_inc_reg" opcode="0x46"/>
  <instruction func="op_inc_reg" opcode="0x47"/>
*/


int op_inc_reg(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->len += 1;
  new->instr = ASM_INC;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_INCDEC | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_OPMOD, new);

  return (new->len);
}
