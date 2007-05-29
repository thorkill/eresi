/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_debug.c,v 1.2 2007-05-29 00:40:28 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>


/**
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_debug(asm_operand *operand, u_char *opcode, int otype,
				asm_instr *ins)
{ 
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_DEBUG;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_CREG;
  operand->base_reg = modrm->r;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (1);
}