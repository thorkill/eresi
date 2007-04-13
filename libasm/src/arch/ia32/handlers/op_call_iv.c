/*
** $Id: op_call_iv.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_call_iv" opcode="0xe8"/>
*/

int op_call_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  int	olen;
    new->ptr_instr = opcode;
  new->instr = ASM_CALL;
  new->type = ASM_TYPE_CALLPROC;
  new->len += 1;
  #if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_JUMP, proc));  
#else
  new->op1.type = ASM_OTYPE_JUMP;
  new->len += asm_proc_vector_len(proc);
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.imm = 0;
  memcpy(&new->op1.imm, opcode + 1, 4);
#endif
  return (new->len);
}
