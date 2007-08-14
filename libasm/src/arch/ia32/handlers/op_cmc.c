/*
** $Id: op_cmc.c,v 1.4 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmc" opcode="0xf5"/>
*/

int op_cmc(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMC;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF;
  return (new->len);
}
