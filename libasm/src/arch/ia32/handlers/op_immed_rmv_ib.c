/*
** $Id: op_immed_rmv_ib.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmv_ib" opcode="0x83"/>
*/

int op_immed_rmv_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  int			olen;
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) (opcode + 1);
  new->ptr_instr = opcode;
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
      break;
    } 

  #if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, proc));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op1.size = ASM_OSIZE_VECTOR;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
  operand_rmv_ib(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
