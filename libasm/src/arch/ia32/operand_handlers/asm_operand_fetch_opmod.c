/**
 * @file asm_operand_fetch_opmod.c
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Decode data for operand type ASM_CONTENT_YDEST
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_opmod(asm_operand *operand, u_char *opcode, 
				int type, asm_instr *ins)
{
  struct s_modrm        *modrm;
  operand->content = ASM_CONTENT_OPMOD;
  operand->type = ASM_OPTYPE_REG;
  operand->regset = asm_proc_opsize(ins->proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  modrm = (struct s_modrm *) opcode;
  operand->baser = modrm->m;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  return (0);
}
