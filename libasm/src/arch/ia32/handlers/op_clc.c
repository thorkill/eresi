/*
** $Id: op_clc.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_clc" opcode="0xf8"/>
*/

int op_clc(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLC;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF;
  return (new->len);
}
