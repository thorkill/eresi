/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_control.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>


int     asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int otype, 
                                  asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_CONTROL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_CREG;
  operand->base_reg = modrm->r;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (1);
}
