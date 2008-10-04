/**
 * @file asm_operand_fetch_general.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_general.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 * @brief Operand Handler to decode data for operand type ASM_OTYPE_GENERAL
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_OTYPE_GENERAL
 * @ingroup operand_handler
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param type Not used.
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_general(asm_operand *operand, unsigned char *opcode, 
				  int type, asm_instr *ins, int opt)
#else
int     asm_operand_fetch_general(asm_operand *operand, unsigned char *opcode, 
				  int type, asm_instr *ins)
#endif
{
  struct s_modrm        *modrm;
  
  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(ins->proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  modrm = (struct s_modrm *) opcode;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  return (operand->len = 0);
}
