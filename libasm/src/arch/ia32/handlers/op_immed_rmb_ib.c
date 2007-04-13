/*
** $Id: op_immed_rmb_ib.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_immed_rmb_ib" opcode="0x80"/>
  <instruction func="op_immed_rmb_ib" opcode="0x82"/>
*/

int op_immed_rmb_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  int			olen;
  struct s_modrm        *modrm;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len+= 1;

#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, proc));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_ENCODED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
#endif
  switch(modrm->r) 
    {
    case 0:
      new->instr = ASM_ADD;
      break;
    case 1:
      new->instr = ASM_ORB;
      new->op2.imm &= 0xff;
      break;
    case 2:
      new->instr = ASM_ADC;
      break;
    case 3:
      new->instr = ASM_SBB;
      break;
    case 4:
      new->instr = ASM_AND;
      new->op2.imm &= 0xff;
      break;
    case 5:
      new->instr = ASM_SUB;
      break;
    case 6:
      new->instr = ASM_XOR;
      break;
    case 7:
      new->instr = ASM_CMP;
      new->op2.imm &= 0xff;
      break;
    }
  return (new->len);
}
