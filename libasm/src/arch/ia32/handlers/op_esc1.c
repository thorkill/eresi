/*
** $Id: op_esc1.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc1" opcode="0xd9"/>
*/

int op_esc1(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  struct s_modrm        *modrm;
  
  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->mod == 3)
    switch(modrm->r) 
      {
    case 0:
      new->instr = ASM_FLD;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
      new->len += 1;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.len = 1;
      new->op1.scale = modrm->m;
#else
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.len = 1;
      new->op1.scale = modrm->m;
#endif
      break;
    case 1:
      new->instr = ASM_FXCH;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, proc);
      new->len += 1;
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.len = 1;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.scale = modrm->m;      
#else
      new->op1.type = ASM_OTYPE_FIXED;
      new->op1.len = 1;
      new->op1.content = ASM_OP_FPU | ASM_OP_SCALE | ASM_OP_BASE;
      new->op1.scale = modrm->m;
#endif
      break;
    case 2:
      new->instr = ASM_FNOP; break;
    case 4:
      switch(modrm->m) {
      case 0: new->instr = ASM_FCHS; break;
      case 1: new->instr = ASM_FABS; break;
      case 4: new->instr = ASM_FTST; break;
      case 5: new->instr = ASM_FXAM; break;
      default: new->instr = ASM_BAD; break;
      }
      break;
    case 5:
      switch(modrm->m) {
      case 0: new->instr = ASM_FLD1; break;
      case 1: new->instr = ASM_FLDL2T; break;
      case 2: new->instr = ASM_FLDL2E; break;
      case 3: new->instr = ASM_FLDPI; break;
      case 4: new->instr = ASM_FLDLG2; break;
      case 5: new->instr = ASM_FLDLN2; break;
      case 6: new->instr = ASM_FLDZ; break;
      }
      break;
    case 6:
      switch(modrm->m) {
      case 0: new->instr = ASM_F2XM1; break;
      case 1: new->instr = ASM_FYL2X; break;
      case 2: new->instr = ASM_FPTAN; break;
      case 3: new->instr = ASM_FPATAN; break;
      case 4: new->instr = ASM_FXTRACT; break;
	//case 5: new->instr = ASM_FPREM1; break;
      case 6: new->instr = ASM_FDECSTP; break;
      case 7: new->instr = ASM_FINCSTP; break;
      }
      break;
    case 7:
      switch(modrm->m) {
      case 0: new->instr = ASM_FPREM; break;
      case 1: new->instr = ASM_FYL2XP1; break;
      case 2: new->instr = ASM_FSQRT; break;
      case 3: new->instr = ASM_FSINCOS; break;
      case 4: new->instr = ASM_FRNDINT; break;
      case 5: new->instr = ASM_FSCALE; break;
	//case 6: new->instr = ASM_SIN; break;
	//case 7: new->instr = ASM_COS; break;
      }
      break;
    }
  else /* modrm != 3 */
    switch(modrm->r) 
      {
      case 0:
	new->instr = ASM_FLD;
	break;
      case 1:
	new->instr = ASM_BAD;
	break;
      case 2:
	new->instr = ASM_FST;
	break;
      case 3:
	new->instr = ASM_FSTP;
	break;
      case 4:
	new->instr = ASM_FLDENV;
	break;
      case 5:
	new->instr = ASM_FLDCW;
	break;
      case 6:
	new->instr = ASM_FNSTENV;
	break;
      case 7:
	new->instr = ASM_FNSTCW;
	break;
      }

  if (modrm->mod < 3) 
    {
      #if LIBASM_USE_OPERAND_VECTOR
      new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
				    proc);
      #else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmv(&new->op1, opcode + 1, len - 1, proc);
#endif
    } 
  
#if LIBASM_USE_OPERAND_VECTOR
#else
  if (new->op1.type)
    new->len += new->op1.len;
  else
    new->len += 1;
#endif
  return (new->len);
}
