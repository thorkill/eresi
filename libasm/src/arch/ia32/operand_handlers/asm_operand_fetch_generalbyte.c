/**
 * @file asm_operand_fetch_generalbyte.c
 * @ingroup operand_handler
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Decode data for operand type ASM_CONTENT_GENERALBYTE
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, 
				      int type, asm_instr *ins)
{ 
  struct s_modrm        *modrm;
  
  operand->content = ASM_CONTENT_GENERAL;
  operand->type = ASM_OPTYPE_REG;
  operand->regset = ASM_REGSET_R8;
  modrm = (struct s_modrm *) opcode;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->sindex = get_reg_intel(operand->indexr, operand->regset);
  return (operand->len = 0);
}
