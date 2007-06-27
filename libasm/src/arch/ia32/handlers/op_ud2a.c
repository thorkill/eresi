/**
 * @file op_ud2a.c
 * @ingroup handlers_ia32
 * $Id: op_ud2a.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
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
