/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc0" opcode="0xd8"/>
*/

int op_esc0(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r) {
  case 0:
    new->instr = ASM_FADD;

    break;
  case 1:
    new->instr = ASM_FMUL;
    break;
  case 2:
    new->instr = ASM_FCOM;
    break;
  case 3:
    new->instr = ASM_FCOMP;
    break;
  case 4:
    new->instr = ASM_FSUB;
    break;
  case 5:
    new->instr = ASM_FSUBR;
    break;
  case 6:
    new->instr = ASM_FDIV;
    break;
  case 7:
    new->instr = ASM_FDIVR;
    break;
  }
  if (modrm->mod < 3)
    {
      #if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED, new);
      #else
      new->op[0].content = ASM_CONTENT_FIXED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
      new->len += new->op[0].len;
      #endif
    }
  else
    {
      new->len += 1;
      switch(modrm->r)
	{
	case 6:
	case 0:
	case 5:
	case 1:
	case 4:
	case 7:
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FPU, new);
	  new->op[0].type = ASM_OPTYPE_REG;
	  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FPU_SCALED, new);
	  new->op[1].type = ASM_OPTYPE_REG;
	  new->op[1].scale = modrm->m;
	  break;

	case 2:
	case 3:
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FPU_SCALED, new);
	  new->op[0].type = ASM_OPTYPE_REG;
	  new->op[0].scale = modrm->m;
	  break;
	}
    }
  return (new->len);
}
