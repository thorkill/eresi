/*
** $Id: op_arpl_ew_rw.c,v 1.2 2007-04-13 06:56:34 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_arpl_ew_rw" opcode="0x63"/>
*/

int     op_arpl_ew_rw(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_ARPL;
  new->len += 1;
  
  /*
    new->op1 = &new->op1;
  new->op1.type = ;
  new->op1.
  
  new->op2 = &new->op2;
  new->op2.content = ;
  new->op2.len = ;
  new->op2.ptr = ;
  */
    #if LIBASM_USE_OPERAND_VECTOR
#endif

  return (new->len);
}
