/**
 * @file op_xlatb.c
 * @ingroup handlers_ia32
 * $Id: op_xlatb.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction opcode="0xd7" func="op_xlatb"/>
*/

int op_xlatb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_XLATB;
  return (new->len);
}
