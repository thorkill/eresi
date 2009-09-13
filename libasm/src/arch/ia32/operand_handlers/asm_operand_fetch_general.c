/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_general.c
 *
 * @ingroup IA32_operands
 * $Id$
 * @brief Operand Handler to decode data for operand type ASM_OTYPE_GENERAL
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_OTYPE_GENERAL
 *
 * @ingroup IA32_operands
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param type Not used.
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_general(asm_operand *operand, u_char *opcode, 
				  int type, asm_instr *ins, int opt)
#else
int     asm_operand_fetch_general(asm_operand *operand, u_char *opcode, 
				  int type, asm_instr *ins)
#endif
{
  struct s_modrm        *modrm;
  
  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_is_protected(ins->proc) ? 
    ASM_REGSET_R32 : ASM_REGSET_R16;
  modrm = (struct s_modrm *) opcode;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->len = 0;
  return (0);
}
