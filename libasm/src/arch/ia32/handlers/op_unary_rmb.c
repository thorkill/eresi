/**
 * @file op_unary_rmb.c
* $Id: op_unary_rmb.c,v 1.3 2007-05-29 00:40:28 heroine Exp $
*
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_unary_rmb" opcode="0xf6"/>
*/

int op_unary_rmb(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc) 
{
  struct s_modrm        *modrm;
  int			olen;
  
  new->ptr_instr = opcode;
  modrm = (struct s_modrm *) opcode + 1;
  new->len += 1;
    switch(modrm->r) {
    case 0:
      new->instr = ASM_TEST;
      new->op1.type = ASM_OTYPE_ENCODED;
#if LIBASM_USE_OPERAND_VECTOR
      new->len += (olen = asm_operand_fetch(&new->op1, opcode + 1, 
					    ASM_OTYPE_ENCODEDBYTE, new));
      new->len += asm_operand_fetch(&new->op2, opcode + 1 + olen, 
				    ASM_OTYPE_IMMEDIATEBYTE, new);
#else
      operand_rmb(&new->op1, opcode + 1, len - 1, proc);
      new->op2.type = ASM_OTYPE_IMMEDIATE;
      new->op2.size = ASM_OSIZE_BYTE;
      new->op2.content = ASM_OP_VALUE;
      new->op2.len = 1;
      new->op2.ptr = opcode + 1 + new->op1.len;
      new->op2.imm = 0;
      memcpy(&new->op2.imm, new->op2.ptr, 1);
      new->len += new->op1.len + new->op2.len;
      #endif
      break;
    case 1:
      return (0);
      break;
    case 2:
      new->instr = ASM_NOT;
      break;
    case 3:
      new->instr = ASM_NEG;
      break;
    case 4:
      new->instr = ASM_MUL;
      break;
    case 5:
      new->instr = ASM_IMUL;
      break;
    case 6:
      new->instr = ASM_DIV;
      break;
    case 7:
      new->instr = ASM_IDIV;
      break;
    default:
      break;
    }
    if (!new->op1.type) 
      {
#if LIBASM_USE_OPERAND_VECTOR
	new->len += asm_operand_fetch(&new->op1, opcode + 1, 
				      ASM_OTYPE_ENCODEDBYTE, new);
      #else
      new->op1.type = ASM_OTYPE_ENCODED;
      operand_rmb(&new->op1, opcode + 1, len - 1, proc);
      new->len += new->op1.len;
      #endif
    }
  return (new->len);
}
