/*
** $Id: op_mov_rmv_rv.c,v 1.6 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rmv_rv" opcode="0x89"/>
*/

int op_mov_rmv_rv(asm_instr *new, unsigned char *opcode, unsigned int len,
                  asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->len += 1;
  new->instr = ASM_MOV;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,                                new);
#endif

  if (new->op[0].content == ASM_OP_BASE &&
      new->op[0].baser == ASM_REG_EBP &&
      new->op[1].baser == ASM_REG_ESP) {

    new->type |= ASM_TYPE_PROLOG;
  }
  else if (new->op[0].content == ASM_OP_BASE &&
            new->op[0].baser == ASM_REG_ESP &&
            new->op[1].baser == ASM_REG_EBP) {

    new->type |= ASM_TYPE_EPILOG;
  }

  return (new->len);
}
