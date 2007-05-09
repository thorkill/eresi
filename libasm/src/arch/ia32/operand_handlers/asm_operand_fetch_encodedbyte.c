/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_encodedbyte.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch ASM_OTYPE_ENCODED operand
 *
 *
 */

int     asm_operand_fetch_encodedbyte(asm_operand *operand, u_char *opcode, int otype,
				      asm_processor *proc)
{ 
  operand->type = ASM_OTYPE_ENCODED;
  return (operand_rmb(operand, opcode, 5, proc));
}
