/**
 * @file op_ud2a.c
 * $Id: op_ud2a.c,v 1.2 2007-04-13 06:56:35 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

int     op_ud2a(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_UD2A;
  #if LIBASM_USE_OPERAND_VECTOR
  #endif
  return (new->len);
}
