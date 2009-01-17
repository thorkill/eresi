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
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1,				    ASM_CONTENT_ENCODED, new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1,				    ASM_CONTENT_ENCODED, new);
#endif
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
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED,					new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED,					new);
#endif
	  new->op[0].type = ASM_OP_FPU | ASM_OP_BASE;
#if WIP
	  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED,					new, 0);
#else
	  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_CONTENT_FIXED,					new);
#endif
	  new->op[1].type = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
	  new->op[1].scale = modrm->m;
#else
	  new->op[0].content = ASM_CONTENT_FIXED;
	  new->op[0].type = ASM_OP_FPU | ASM_OP_BASE;
	  new->op[1].content = ASM_CONTENT_FIXED;
	  new->op[1].type = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
	  new->op[1].scale = modrm->m;
#endif
	  break;

	case 2:
	case 3:
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new,
					asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
						       asm_proc_is_protected(proc) ?
						       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
	  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_CONTENT_FIXED, new);
#endif
	  new->op[0].type = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
	  new->op[0].scale = modrm->m;
#else
	  new->op[0].content = ASM_CONTENT_FIXED;
	  new->op[0].type = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
	  new->op[0].scale = modrm->m;
#endif
	  break;
	}
    }
  return (new->len);
}
