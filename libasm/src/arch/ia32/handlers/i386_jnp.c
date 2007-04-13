/*
** $Id: i386_jnp.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
<i386 func="i386_jnp" opcode="0x8b"/>
*/


int i386_jnp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_NOT_PARITY;
    new->len += 1;

    #if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_JUMP, proc);
    #else
  
  new->op1.type = ASM_OTYPE_JUMP;
  new->op1.content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  new->op1.ptr = opcode + 1;
  new->op1.len = 4;
  memcpy(&new->op1.imm, opcode + 1, 4);
  new->len += 4;
  #endif
return (new->len);
}
