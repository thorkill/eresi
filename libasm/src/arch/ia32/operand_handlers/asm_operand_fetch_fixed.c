/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_fixed.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */

int     asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, int otype, 
				asm_processor *proc)
{
  operand->type = ASM_OTYPE_FIXED;
  return (0);
}
