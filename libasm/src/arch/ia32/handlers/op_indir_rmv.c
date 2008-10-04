/*
** $Id: op_indir_rmv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_indir_rmv" opcode="0xff"/>
*/

int op_indir_rmv(asm_instr *new, unsigned char *opcode, unsigned int len,
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
    new->op[0].type = ASM_OTYPE_ENCODED;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 1:
    new->instr = ASM_DEC;
    new->type = ASM_TYPE_INCDEC | ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
    new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_SF | ASM_FLAG_ZF;
    new->op[0].type = ASM_OTYPE_ENCODED;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 2:
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
    new->spdiff = -4;
    new->instr = ASM_CALL;
    new->op[0].type = ASM_OTYPE_MEMORY;
    break;
  case 3:
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
    new->instr = ASM_CALL;
    new->op[0].type = ASM_OTYPE_MEMORY;

    break;
  case 4:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op[0].type = ASM_OTYPE_MEMORY;
    break;
  case 5:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    new->op[0].type = ASM_OTYPE_MEMORY;
    break;
  case 6:
    new->instr = ASM_PUSH;
    new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
    new->op[0].type = ASM_OTYPE_ENCODED;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 7:
    new->instr = ASM_BAD;
    new->op[0].type = ASM_OTYPE_ENCODED;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  default:
    break;
  } /* switch */
  if ((new->op[0].type == ASM_OTYPE_ENCODED) ||
      (new->op[0].type == ASM_OTYPE_MEMORY)) {
#if WIP
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				  new, 0);
#else
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				  new);
#endif
    if (new->op[0].type == ASM_OTYPE_MEMORY)
      new->op[0].content |= ASM_OP_ADDRESS;
  }
  return (new->len);
}
