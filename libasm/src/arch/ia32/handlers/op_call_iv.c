/*
** $Id: op_call_iv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_call_iv" opcode="0xe8"/>
*/

int op_call_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {

    new->ptr_instr = opcode;
  new->instr = ASM_CALL;
  new->type = ASM_TYPE_CALLPROC;
  new->op1.type = ASM_OTYPE_JUMP;
  new->len += 1 + asm_proc_vector_len(proc);
  
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  
  new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 4);
  return (new->len);
}
