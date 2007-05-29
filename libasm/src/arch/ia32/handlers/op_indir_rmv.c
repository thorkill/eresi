/*
** $Id: op_indir_rmv.c,v 1.4 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_indir_rmv" opcode="0xff"/>
*/

int op_indir_rmv(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc)
{
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
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
    new->spdiff = -4;
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
#if LIBASM_USE_OPERAND_VECTOR
    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED,
				  new);
#else
    operand_rmv(&new->op1, opcode + 1, len - 1, proc);
#endif
    if (new->op1.type == ASM_OTYPE_MEMORY)
      new->op1.content |= ASM_OP_ADDRESS;
  }
#if LIBASM_USE_OPERAND_VECTOR
#else
  new->len += new->op1.len;  
#endif
  return (new->len);
}
