/**
 * @file asm_operand_fetch_generalbyte.c
ngroup ia-operands/g
 * @ingroup operands
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Decode data for operand type ASM_OTYPE_GENERALBYTE
ngroup ia-operands/g
 * @ingroup operands
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, 
				      int type, asm_instr *ins)
#endif
{ 
  struct s_modrm        *modrm;
  
  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_R8;
  modrm = (struct s_modrm *) opcode;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->sindex = get_reg_intel(operand->indexr, operand->regset);
  return (operand->len = 0);
}
