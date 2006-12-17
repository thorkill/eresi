/*
** Author  : <strauss at rfdslabs com br>
** Started : Wed Dec  17 02:07:11 2006
** Updated : Wed Dec  17 02:07:11 2006
*/

#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_group12" opcode="0x71"/>
 */

int	i386_group12(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  struct s_modrm *modrm = (struct s_modrm *) opcode + 1;
  
  switch (modrm->r) {
    case 2:
	  new->instr = ASM_PSRLW;
      new->op1.type = ASM_OTYPE_PMMX;
      new->op1.size = ASM_OSIZE_QWORD;
  	  new->op2.type = ASM_OTYPE_IMMEDIATE;
	  new->op2.size = ASM_OSIZE_BYTE;

	  operand_rmb_ib(new, opcode + 1, len - 1, proc);
	  new->op1.regset = ASM_REGSET_MM;
	  break;
	  
    case 4:
	  new->instr = ASM_PSRAW;
      new->op1.type = ASM_OTYPE_PMMX;
      new->op1.size = ASM_OSIZE_QWORD;
  	  new->op2.type = ASM_OTYPE_IMMEDIATE;
	  new->op2.size = ASM_OSIZE_BYTE;

	  operand_rmb_ib(new, opcode + 1, len - 1, proc);
	  new->op1.regset = ASM_REGSET_MM;
	  break;

	case 6:
	  new->instr = ASM_PSLLW;
      new->op1.type = ASM_OTYPE_PMMX;
      new->op1.size = ASM_OSIZE_QWORD;
  	  new->op2.type = ASM_OTYPE_IMMEDIATE;
	  new->op2.size = ASM_OSIZE_BYTE;

	  operand_rmb_ib(new, opcode + 1, len - 1, proc);
	  new->op1.regset = ASM_REGSET_MM;
	  break;
	  
	default:
	  new->instr = ASM_BAD;
	  new->len = 0;
	  break;
  }

  return (new->len);
}

/*
  <i386 func="i386_group14" opcode="0x73"/>
 */

int	i386_group14(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  struct s_modrm *modrm = (struct s_modrm *) opcode + 1;
  
  switch (modrm->r) {
    case 2:
	  new->instr = ASM_PSRLQ;
      new->op1.type = ASM_OTYPE_PMMX;
      new->op1.size = ASM_OSIZE_QWORD;
  	  new->op2.type = ASM_OTYPE_IMMEDIATE;
	  new->op2.size = ASM_OSIZE_BYTE;

	  operand_rmb_ib(new, opcode + 1, len - 1, proc);
	  new->op1.regset = ASM_REGSET_MM;
	  break;

	case 6:
	  new->instr = ASM_PSLLQ;
      new->op1.type = ASM_OTYPE_PMMX;
      new->op1.size = ASM_OSIZE_QWORD;
  	  new->op2.type = ASM_OTYPE_IMMEDIATE;
	  new->op2.size = ASM_OSIZE_BYTE;

	  operand_rmb_ib(new, opcode + 1, len - 1, proc);
	  new->op1.regset = ASM_REGSET_MM;
	  break;
	  
	default:
	  new->instr = ASM_BAD;
	  new->len = 0;
	  break;
  }

  return (new->len);
}

/*
  <i386 func="i386_emms" opcode="0x77"/>
 */

int	i386_emms(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOVQ;

  return (new->len);
}

/*
  <i386 func="i386_movq_qq_pq" opcode="0x7f"/>
 */

int	i386_movq_qq_pq(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVQ;

  new->op1.type = ASM_OTYPE_QMMX;
  new->op1.size = ASM_OSIZE_QWORD;
  new->op2.type = ASM_OTYPE_PMMX;
  new->op2.size = ASM_OSIZE_QWORD;

  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op2.regset = ASM_REGSET_MM;

  return (new->len);
}

