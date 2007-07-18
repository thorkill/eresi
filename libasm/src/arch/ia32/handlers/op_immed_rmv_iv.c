/*
** $Id: op_immed_rmv_iv.c,v 1.5 2007-07-18 15:47:10 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmv_iv" opcode="0x81"/>
 */

int op_immed_rmv_iv(asm_instr *new, u_char *opcode, u_int len, 
		    asm_processor *proc) 
{
  int			olen;
  struct s_modrm        *modrm;
  new->ptr_instr = opcode;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_OR;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      new->type = ASM_TYPE_COMPARISON | ASM_TYPE_CONTROL;
      break;
  }

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, 
					ASM_OTYPE_ENCODED, new));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, 
				ASM_OTYPE_IMMEDIATE, new);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmv_iv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
