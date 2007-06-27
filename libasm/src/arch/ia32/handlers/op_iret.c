/**
 * @file op_iret.c
 * @ingroup handlers_ia32
 * $Id: op_iret.c,v 1.4 2007-06-27 11:25:11 heroine Exp $
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
