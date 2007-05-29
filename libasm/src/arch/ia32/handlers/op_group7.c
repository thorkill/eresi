/*
** $Id: op_group7.c,v 1.3 2007-05-29 00:40:27 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

int     op_group7(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc)
{ 
  struct s_modrm        *modrm;
  
  new->len += 1;
  modrm = (struct s_modrm *) opcode + 1;

  switch(modrm->r) 
    {
    case 0:
      new->instr = ASM_SGDT;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				    new);
#else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
#endif
      break;
    case 1:
      new->instr = ASM_SIDT;
      break;
    case 2:
      new->instr = ASM_LGDT;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				    new);
#else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
#endif
      break;
    case 3:
      new->instr = ASM_LIDT;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				    new);
#else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
#endif
      break;
    case 4:
      break;
    case 5:
      new->instr = ASM_BAD;
      break;
    case 6:
      
      break;
    case 7:

      break;
    }
  return (new->len);
}
