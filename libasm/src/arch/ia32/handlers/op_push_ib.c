/*
** $Id: op_push_ib.c,v 1.4 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_push_ib" opcode="0x6a"/>
*/

int op_push_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->instr = ASM_PUSH;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_TOUCHSP;
  new->spdiff = -4;
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				ASM_OTYPE_IMMEDIATEBYTE, new);
#else
  new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  new->op1.ptr = opcode + 1;
  new->op1.len = 1;
  
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy(&new->op1.imm, opcode + 1, 1);
  new->len += 1;
#endif
  return (new->len);
}
