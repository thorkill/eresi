/**
 * @file op_mvo_subreg_ib.c
 * @ingroup handlers_ia32
 * $Id: op_mov_subreg_ib.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_subreg_ib" opcode="0xb0"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb1"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb2"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb3"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb4"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb5"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb6"/>
  <instruction func="op_mov_subreg_ib" opcode="0xb7"/>

*/

int op_mov_subreg_ib(asm_instr *new, u_char *opcode, u_int len, 
                     asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->instr = ASM_MOV;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_OPMOD, new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1,
                                ASM_OTYPE_IMMEDIATEBYTE, new);

  return (new->len);
}
