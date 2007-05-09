/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_generalbyte.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_OTYPE_GENERAL operand
 *
 */

int     asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, int type, asm_processor *proc)
{ 
  struct s_modrm        *modrm;
  
  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_R8;
  modrm = (struct s_modrm *) opcode;
  operand->base_reg = modrm->r;
  return (operand->len = 0);
}
