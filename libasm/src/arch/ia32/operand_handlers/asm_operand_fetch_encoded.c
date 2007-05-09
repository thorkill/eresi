/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_encoded.c,v 1.1 2007-05-09 17:39:35 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>


/**
 * Fetch ASM_OTYPE_ENCODED operand
 *
 *
 */

int     asm_operand_fetch_encoded(asm_operand *operand, u_char *opcode, int otype, 
                                  asm_processor *proc)
{
  operand->type = ASM_OTYPE_ENCODED;
  return (operand_rmv(operand, opcode, 4, proc));
}
