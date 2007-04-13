/**
 * @file op_leave.c
 * $Id: op_leave.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the leave instruction opcode 0xc9
 *
  <instruction func="op_leave" opcode="0xc9"/>
*/


int op_leave(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  //new->type = ASM_TYPE_RETPROC;
  new->instr = ASM_LEAVE;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
return (new->len);
}
