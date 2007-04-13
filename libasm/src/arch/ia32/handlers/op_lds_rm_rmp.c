/*
** $Id: op_lds_rm_rmp.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction lds opcode 0xc5
 *
 <instruction func="op_lds_rm_rmp" opcode="0xc5"/>
*/

int op_lds_rm_rmp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
    new->instr = ASM_LDS;
    new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
#endif
  return (new->len);
}
