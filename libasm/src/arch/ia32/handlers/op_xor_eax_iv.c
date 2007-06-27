/**
 * @file op_xor_eax_iv.c
 * @ingroup handlers_ia32
** $Id: op_xor_eax_iv.c,v 1.4 2007-06-27 11:25:12 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_xor_eax_iv" opcode="0x35"/>
*/


int op_xor_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
		  asm_processor *proc) 
{
  new->instr = ASM_XOR;
  new->ptr_instr = opcode;
  new->len += 1;

  LIBASM_PROFILE_FIN();
    
#if LIBASM_USE_OPERAND_VECTOR
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_IMMEDIATE, 
				new);
#else
  new->op1.type = ASM_OTYPE_FIXED;
  new->op2.type = ASM_OTYPE_IMMEDIATE;

  new->op1.content = ASM_OP_BASE | ASM_OP_FIXED;
  new->op1.ptr = opcode;
  new->op1.len = 0;
  new->op1.baser = ASM_REG_EAX;
  new->op1.regset = asm_proc_is_protected(proc) ?
    ASM_REGSET_R32 : ASM_REGSET_R16;

  new->op2.content = ASM_OP_VALUE;
  new->op2.ptr = opcode + 1;
  new->op2.len = asm_proc_vector_len(proc);
  new->op2.imm = 0;
  memcpy(&new->op2.imm, opcode + 1, asm_proc_vector_len(proc));
  new->len += asm_proc_vector_len(proc);
#endif
  LIBASM_PROFILE_FOUT(new->len);
  return (new->len);
}

