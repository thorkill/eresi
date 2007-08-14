/*
** $Id: op_aaa.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the aaa instruction, opcode 0x37
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of disassembled instruction.
*/

int     op_aaa(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_AAA;
  new->type = ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG | ASM_TYPE_ARITH;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF;
  new->flagsread = ASM_FLAG_AF;
  return (new->len);
}
