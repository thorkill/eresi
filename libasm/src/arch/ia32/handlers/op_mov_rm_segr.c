/*
** $Id: op_mov_rm_segr.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rm_segr" opcode="0x8c"/>
*/

int op_mov_rm_segr(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode + 1;
  new->type = ASM_TYPE_ASSIGN;
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOV;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODED, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_SEGMENT, 
                                new);

  return (new->len);
}
