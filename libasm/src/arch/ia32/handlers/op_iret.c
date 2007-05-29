/**
 * @file op_iret.c
 * $Id: op_iret.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
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
  return (new->len);
}
