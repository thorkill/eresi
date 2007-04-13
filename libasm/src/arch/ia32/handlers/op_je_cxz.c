/*
** $Id: op_je_cxz.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_je_cxz" opcode="0xe3"/>
 */

int op_je_cxz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_CXZ;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_SHORTJUMP, proc);
  #else
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.size = ASM_OSIZE_BYTE;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.ptr = opcode + 1;
  
  new->op1.imm = 0;
  if (*(opcode + 1) > 0x80u)
    memcpy((char *) &new->op1.imm + 1, "\xff\xff\xff", 3);
  memcpy((char *) &new->op1.imm, opcode + 1, 1);
  new->len += 1;
  #endif
  // new->type = IS_COND_BRANCH;
  return (new->len);
}
