/*
** $Id: i386_group12.c,v 1.3 2007-05-11 10:48:29 may Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_group12" opcode="0x71"/>
*/

int			i386_group12(asm_instr *new, u_char *opcode, 
				     u_int len, asm_processor *proc) 
{
  int			olen;
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  switch (modrm->r) 
    {
    case 2:
      new->instr = ASM_PSRLW;

      break;

    case 4:
      new->instr = ASM_PSRAW;
      break;
        
    case 6:
      new->instr = ASM_PSLLW;
      break;
	  
    default:
      new->instr = ASM_BAD;
      return (new->len = 0);
      break;
    }
#if LIBASM_USE_OPERAND_VECTOR
  new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_PMMX,
					proc));
  new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, ASM_OTYPE_IMMEDIATEBYTE, proc);
#else
  new->op1.type = ASM_OTYPE_PMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_IMMEDIATE;
  new->op2.size = ASM_OSIZE_BYTE;
      
  operand_rmb_ib(new, opcode + 1, len - 1, proc);
  new->op1.regset = ASM_REGSET_MM;
#endif
  
  return (new->len);
}
