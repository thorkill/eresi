/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
   <instruction func="op_esc2" opcode="0xda"/>
*/

int op_esc2(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->ptr_instr = opcode;
  new->len += 1;
  if (modrm->mod == 3)
    switch (modrm->r) {
    case 5: new->instr = ASM_FUCOMPP; break;
    default: new->instr = ASM_BAD; break;
    }
  else
    switch(modrm->r) {
    case 0:
      new->instr = ASM_FIADD;
      break;
    case 1:
      new->instr = ASM_FIMUL;
      break;
    case 2:
      new->instr = ASM_FICOM;
      break;
    case 3:
      new->instr = ASM_FICOMP;
      break;
    case 4:
      new->instr = ASM_FISUB;
      break;
    case 5:
      new->instr = ASM_FISUBR;
      break;
    case 6:
      new->instr = ASM_FIDIV;
      break;
    case 7:
      new->instr = ASM_FIDIVR;
      break;
    }
  if (!(*(opcode + 1) == 0xe9)) {
    #if LIBASM_USE_OPERAND_VECTOR
#if WIP
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				  new, 0);
#else
    new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				  new);
#endif
    #else
    new->op[0].content = ASM_CONTENT_ENCODED;
    operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
    new->len += new->op[0].len;
    #endif
  } else
    new->len += 1;
  return (new->len);
}
