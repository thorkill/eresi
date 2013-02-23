/*
** $Id$
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
    new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF | ASM_FLAG_SF | ASM_FLAG_ZF;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 1:
    new->instr = ASM_DEC;
    new->type = ASM_TYPE_INCDEC | ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
    new->flagswritten = ASM_FLAG_AF | ASM_FLAG_OF | ASM_FLAG_PF | ASM_FLAG_SF | ASM_FLAG_ZF;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 2:
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
    new->spdiff = -4;
    new->instr = ASM_CALL;
    break;
  case 3:
    new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
    new->instr = ASM_CALL;
    break;
  case 4:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    break;
  case 5:
    new->type = ASM_TYPE_IMPBRANCH;
    new->instr = ASM_BRANCH;
    break;
  case 6:
    new->instr = ASM_PUSH;
    new->type = ASM_TYPE_TOUCHSP | ASM_TYPE_STORE;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  case 7:
    new->instr = ASM_BAD;
    new->op[0].size = ASM_OSIZE_VECTOR;
    break;
  default:
    break;
  } /* switch */

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED, new);
  if (new->op[0].type == ASM_OPTYPE_MEM)
    new->op[0].memtype |= ASM_OP_ADDRESS;

  return (new->len);
}
