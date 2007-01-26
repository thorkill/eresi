/*
** $Id: op_indir_rmv.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_indir_rmv" opcode="0xff"/>
*/

int op_indir_rmv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  struct s_modrm        *modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) (opcode + 1);
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_INC;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 1:
    new->instr = ASM_DEC;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 2:
    new->type = ASM_TYPE_CALLPROC;
    new->instr = ASM_CALL;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 3:
    new->type = ASM_TYPE_CALLPROC;
    new->instr = ASM_CALL;
    new->op1.type = ASM_OTYPE_MEMORY;

    break;
  case 4:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 5:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op1.type = ASM_OTYPE_MEMORY;
    break;
  case 6:
    new->instr = ASM_PUSH;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 7:
    new->instr = ASM_BAD;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  default:
    break;
  } /* switch */
  if ((new->op1.type == ASM_OTYPE_ENCODED) || 
      (new->op1.type == ASM_OTYPE_MEMORY)) {
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
    if (new->op1.type == ASM_OTYPE_MEMORY)
      new->op1.content |= ASM_OP_ADDRESS;
  }
  new->len += new->op1.len;  
  return (new->len);
}
