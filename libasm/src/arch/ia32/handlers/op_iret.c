/**
 * @file op_iret.c
 * $Id: op_iret.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
 *
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction opcode="0xcf" func="op_iret"/>
 */

int op_iret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_IRET;
#if LIBASM_USE_OPERAND_VECTOR
    
#endif
  return (new->len);
}
