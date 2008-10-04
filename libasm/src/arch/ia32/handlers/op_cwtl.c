/*
** $Id: op_cwtl.c,v 1.4 2007/08/14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cwtl" opcode="0x98"/>
*/

int     op_cwtl(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ARITH;
  if (asm_proc_opsize(proc))
    new->instr = ASM_CBTW;
  else
    new->instr = ASM_CWTL;
  return (new->len);
}
