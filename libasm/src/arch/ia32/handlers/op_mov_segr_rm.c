/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_segr_rm" opcode="0x8e"/>
*/

int op_mov_segr_rm(asm_instr *new, u_char *opcode, u_int len,
                   asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_MOV;

  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_CONTENT_SEGMENT, new);
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_CONTENT_ENCODED, new);

  return (new->len);
}
