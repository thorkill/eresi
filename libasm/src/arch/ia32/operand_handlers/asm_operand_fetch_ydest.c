/**
 * @file asm_operand_fetch_ydest.c
 * @brief Wrapper to call operand handler.
 * @ingroup operand_handler
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_ydest(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_ydest(asm_operand *operand, unsigned char *opcode, int otype, 
				asm_instr *ins)
#endif
{
#if WIP
  asm_content_pack(operand, ASM_OP_BASE|ASM_OP_REFERENCE,ASM_OTYPE_YDEST);
#else
  operand->type = ASM_OTYPE_YDEST;
  operand->content = ASM_OP_BASE | ASM_OP_REFERENCE;
#endif
  //asm_content_pack()
  operand->baser = ASM_REG_EDI;
  operand->regset = asm_proc_opsize(ins->proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->prefix = ASM_PREFIX_ES;
  return (0);
}
