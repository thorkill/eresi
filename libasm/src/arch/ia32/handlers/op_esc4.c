/**
 * $Id: op_esc4.c,v 1.5 2008/01/07 01:32:56 heroine Exp $
 * @file op_esc4.c
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler of FPU instruction group esc4 opcode 0xdc
  <instruction func="op_esc4" opcode="0xdc"/>
*/

int op_esc4(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r)
    {
    case 0:
      new->instr = ASM_FADD;
      break;
    case 1:
      new->instr = ASM_FMUL;
      break;
    case 2:
      new->instr= ASM_FCOM;
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

  if (modrm->mod == 3)
    {
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new, 
				    asm_fixed_pack(0, ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE, 
						   modrm->m, 0));
      
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
      new->op[0].type = ASM_OTYPE_FIXED;
      new->op[0].content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op[0].len = 1;
      new->op[0].scale = modrm->m;
#if WIP
      new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_FIXED, 0,
				    asm_fixed_pack(0, ASM_OP_FPU | ASM_OP_BASE, 0,
						   asm_proc_is_protected(proc) ?
						   ASM_REGSET_R32 : ASM_REGSET_R16));
#else
      new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
      new->op[1].type = ASM_OTYPE_FIXED;
      new->op[1].content = ASM_OP_FPU | ASM_OP_BASE;
      new->len += 1;
#else
      new->op[0].type = ASM_OTYPE_FIXED;
      new->op[0].content = ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE;
      new->op[0].len = 1;
      new->op[0].scale = modrm->m;
      new->op[1].type = ASM_OTYPE_FIXED;
      new->op[1].content = ASM_OP_FPU | ASM_OP_BASE;
#endif
    }
  else
    {
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED, new, 0);
#else
      new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				    new);
#endif
#else
      new->op[0].type = ASM_OTYPE_FIXED;
      operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
#endif
    }
#if LIBASM_USE_OPERAND_VECTOR
#else
  if (new->op[0].type)
    new->len += new->op[0].len;
#endif
  return (new->len);
}
