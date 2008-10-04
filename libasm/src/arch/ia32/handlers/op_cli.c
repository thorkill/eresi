/*
** $Id: op_cli.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cli" opcode="0xfa"/>
*/

int op_cli(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLI;
  new->type = ASM_TYPE_WRITEFLAG;
  /* Should be VIF for CPL = 3 and IOPL < CPL */
  new->flagswritten = ASM_FLAG_IF;
  return (new->len);
}
