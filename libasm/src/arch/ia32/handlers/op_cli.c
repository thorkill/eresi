/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cli" opcode="0xfa"/>
*/

int op_cli(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLI;
  new->type = ASM_TYPE_WRITEFLAG;
  /* Should be VIF for CPL = 3 and IOPL < CPL */
  new->flagswritten = ASM_FLAG_IF;
  return (new->len);
}
