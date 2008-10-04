/*
** $Id: op_aaa.c,v 1.7 2007/10/14 00:01:41 heroine Exp $
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

int     op_aaa(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_AAA;
  new->type = ASM_TYPE_WRITEFLAG | ASM_TYPE_READFLAG | ASM_TYPE_ARITH;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF;
  new->flagsread = ASM_FLAG_AF;
  return (new->len);
}
