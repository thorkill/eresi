/*
** $Id: op_retf_i2.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_retf_i2" opcode="0xca"/>
*/

int op_retf_i2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  short *shrt_ptr;

  new->instr = ASM_RETF;
  new->ptr_instr = opcode;
  new->len += 1;
  // new->type = IS_RET;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_IMMEDIATEWORD, proc);
#else
    new->op1.type = ASM_OTYPE_IMMEDIATE;
  new->op1.content = ASM_OP_VALUE;
  shrt_ptr = (short *) (opcode + 1);
  if (*shrt_ptr < 0)
    memcpy((char *) &new->op1.imm + 2, "\xff\xff", 2);
  else
    new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 2);
  new->len += 2;
#endif
  return (new->len);
}
