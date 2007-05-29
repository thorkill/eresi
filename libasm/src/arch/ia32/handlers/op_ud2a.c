/**
 * @file op_ud2a.c
 * $Id: op_ud2a.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

int     op_ud2a(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_UD2A;
  return (new->len);
}
