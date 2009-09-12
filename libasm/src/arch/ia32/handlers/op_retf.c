/**
 * @file op_retf.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_retf" opcode="0xcb"/>
*/

int op_retf(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_RETF;
  new->type = ASM_TYPE_RETPROC | ASM_TYPE_TOUCHSP | ASM_TYPE_EPILOG;
  return (new->len);
}
