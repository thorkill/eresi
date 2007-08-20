/*
** $Id: op_indir_rmv.c,v 1.5 2007-08-20 07:21:04 strauss Exp $
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
    new->type = ASM_TYPE_INCDEC | ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
    new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;
    new->op1.type = ASM_OTYPE_ENCODED;
    new->op1.size = ASM_OSIZE_VECTOR;
    break;
  case 1:
    new->instr = ASM_DEC;
    new->type = ASM_TYPE_INCDEC | ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
    new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;
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
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
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
    new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
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

    new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED,
				  new);

    if (new->op1.type == ASM_OTYPE_MEMORY)
      new->op1.content |= ASM_OP_ADDRESS;
  }

  return (new->len);
}
