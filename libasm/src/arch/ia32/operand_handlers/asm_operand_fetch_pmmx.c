/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_pmmx.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_OTYPE_PMMX operand
 *
 *
 */

int     asm_operand_fetch_pmmx(asm_operand *operand, u_char *opcode, int otype, 
			       asm_processor *proc)
{
  int           len;
  operand->type = ASM_OTYPE_PMMX;
  len = operand_rmv(operand, opcode, 4, proc);
  operand->regset = ASM_REGSET_MM;
  return (len);
}
