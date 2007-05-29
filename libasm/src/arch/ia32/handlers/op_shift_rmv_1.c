/*
** $Id: op_shift_rmv_1.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmv_1" opcode="0xd1"/>
 */

int op_shift_rmv_1(asm_instr *new, u_char *opcode, u_int len, 
		   asm_processor *proc)
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch(modrm->r) {
  case ASM_REG_EDI:
    new->instr = ASM_SAR;
    break;
  case ASM_REG_ESP:
    new->instr = ASM_SHL;
    break;
  default:
    new->instr = ASM_SHR;
    break;
  }
  
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				new);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  operand_rmv(&new->op1, opcode + 1, len - 1, proc);
  
  new->len += new->op1.len;
  #endif
  return (new->len);
}
