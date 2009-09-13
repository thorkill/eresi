/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_control.c
 *
 * @ingroup IA32_operands
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Decode data for operand type ASM_OTYPE_CONTROL
 *
 * @ingroup IA32_operands
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int otype, 
                                  asm_instr *ins)
#endif
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_CONTROL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_CREG;
  operand->baser = modrm->r;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->sindex = get_reg_intel(operand->indexr, operand->regset);
  return (1);
}
