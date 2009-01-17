/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_esc3" opcode="0xdb"/>
*/

int op_esc3(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;

  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
  switch(modrm->r)
    {
  case 0:
    new->instr = ASM_FILD;
    break;
  case 1:
    new->instr = ASM_BAD;
    break;
  case 2:
    new->instr = ASM_FIST;
    break;
  case 3:
    new->instr = ASM_FISTP;
    break;
  case 4:
    // bad
    break;
  case 5:
    new->instr = ASM_FLD;
    break;
  case 6:

  case 7:
    new->instr = ASM_FSTP;
    break;
  }
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_ENCODED,				new);
#endif
#else
  new->op[0].content = ASM_CONTENT_ENCODED;
  operand_rmv(&new->op[0], opcode + 1, len - 1, proc);
  new->len += new->op[0].len;
#endif
  return (new->len);
}
