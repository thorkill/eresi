/*
** $Id: op_lds_rm_rmp.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction lds opcode 0xc5
 *
 <instruction func="op_lds_rm_rmp" opcode="0xc5"/>
*/

int op_lds_rm_rmp(asm_instr *new, unsigned char *opcode, unsigned int len,
		  asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->instr = ASM_LDS;
  new->len += 1;
  new->type = ASM_TYPE_LOAD;
  return (new->len);
}
