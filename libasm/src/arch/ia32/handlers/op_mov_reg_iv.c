/*
** $Id: op_mov_reg_iv.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_reg_iv" opcode="0xb8"/>
  <instruction func="op_mov_reg_iv" opcode="0xb9"/>
  <instruction func="op_mov_reg_iv" opcode="0xba"/>
  <instruction func="op_mov_reg_iv" opcode="0xbb"/>
  <instruction func="op_mov_reg_iv" opcode="0xbc"/>
  <instruction func="op_mov_reg_iv" opcode="0xbd"/>
  <instruction func="op_mov_reg_iv" opcode="0xbe"/>
  <instruction func="op_mov_reg_iv" opcode="0xbf"/>
*/

int op_mov_reg_iv(asm_instr *new, unsigned char *opcode, unsigned int len,
asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;
  new->len += 1;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 0, ASM_OTYPE_OPMOD,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 0, ASM_OTYPE_OPMOD,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new);
#endif

  return (new->len);
}
