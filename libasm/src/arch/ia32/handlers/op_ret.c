/**
 * @file op_ret.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_ret" opcode="0xc3"/>
*/


int op_ret(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP | ASM_TYPE_EPILOG;
  new->spdiff = 4;
  new->instr = ASM_RET;
  return (new->len);
}
