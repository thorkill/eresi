/*
** $Id: op_unary_rmv.c,v 1.1 2007-01-26 14:18:38 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_unary_rmv" opcode="0xf7"/>
*/

int op_unary_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
  new->len += 1;
    switch (modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      break;
    case 1:
      return (0);
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      break;
    case 4:
      new->instr = ASM_MUL;
      break;
    case 5:
      new->instr = ASM_IMUL;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    } /* switch */
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    new->len += new->op1.len;
    if (new->instr == ASM_TEST) {
      new->op2.type = ASM_OTYPE_IMMEDIATE;
      new->op2.len = asm_proc_vector_len(proc);
      new->op2.content = ASM_OP_VALUE;
      new->op2.ptr = opcode + 1 + new->op1.len;
      memcpy(&new->op2.imm, opcode + 1 + new->op1.len, 
             asm_proc_vector_len(proc));
      new->len += new->op2.len;
    }
  return (new->len);
}
