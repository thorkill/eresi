/**
 * @file asm_operand_fetch_segment.c
 * @ingroup operand_handler
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_CONTENT_YDEST
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_segment(asm_operand *operand, u_char *opcode, 
				  int otype, asm_instr *ins)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  
  operand->content = ASM_CONTENT_SEGMENT;
  operand->type = ASM_OPTYPE_REG;

  operand->regset = ASM_REGSET_SREG;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);

  return (0);
}
