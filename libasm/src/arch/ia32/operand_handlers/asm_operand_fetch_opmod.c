/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_opmod.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */

int     asm_operand_fetch_opmod(asm_operand *operand, u_char *opcode, int type, asm_processor *proc)
{
  struct s_modrm        *modrm;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->type = ASM_OTYPE_OPMOD;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  modrm = (struct s_modrm *) opcode;
  operand->base_reg = modrm->m;
  return (0);
}
