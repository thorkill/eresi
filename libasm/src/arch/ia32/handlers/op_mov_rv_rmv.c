/*
** $Id: op_mov_rv_rmv.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rv_rmv" opcode="0x8b"/>
*/

int op_mov_rv_rmv(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ASSIGN;
  new->instr = ASM_MOV;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_GENERAL,
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_ENCODED,
                                new);

  if (new->op2.content == ASM_OP_BASE &&
      new->op1.baser == ASM_REG_EBP &&
      new->op2.baser == ASM_REG_ESP) {

    new->type |= ASM_TYPE_PROLOG;
  }
  else if (new->op2.content == ASM_OP_BASE &&
            new->op1.baser == ASM_REG_ESP &&
            new->op2.baser == ASM_REG_EBP) {

    new->type |= ASM_TYPE_EPILOG;
  }

  return (new->len);
}
