/*
** $Id: output.c,v 1.3 2007-02-26 17:47:15 thor Exp $
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu Mar 11 00:40:31 2004
*/
#include <libasm.h>
#include <libasm-int.h>
/**
 * Resolving function for immediate values.
 * This is a gate to internal handler
 * @param proc Pointer to processor structure
 * @param val Value to resolve
 * @param buf Pointer to store string
 * @param len Length of string
 */
void	asm_resolve_immediate(asm_processor *proc, u_int val, 
			      char *buf, u_int len) 
{
  proc->resolve_immediate(proc->resolve_data, val, buf, len);  
}


/**
 * Ascii Assembly Output function
 * This is a gate to internal handler
 * @param instr Pointer to instruction structure
 * @param addr Virtual address of instruction
 * @return Pointer to a static buffer containing current instruction string
 */
char	*asm_display_instr_att(asm_instr *instr, 
			       int addr) 
{
  return (instr->proc->display_handle(instr, addr));
}





/**
 * Debug dump of an opperand, merged from mjollnir
 */
int	asm_debug_operand(asm_operand * op)
{
  fprintf(stderr, "[*] Operand Dump\n[*] len: %d type: %d size: %d content: %d\n",
	  op->len, op->type, op->size, op->content);
  
  fprintf(stderr, "[*] Content: %s %s %s %s %s %s %s\n",
	  (op->content & ASM_OP_VALUE) ? "ASM_OP_VALUE" : ".",
	  (op->content & ASM_OP_BASE) ? "ASM_OP_BASE" : ".",
	  (op->content & ASM_OP_INDEX) ? "ASM_OP_INDEX" : ".",
	  (op->content & ASM_OP_SCALE) ? "ASM_OP_SCALE" : ".",
	  (op->content & ASM_OP_FIXED) ? "ASM_OP_FIXED" : ".",
	  (op->content & ASM_OP_REFERENCE) ? "ASM_OP_REFERENCE" : ".",
	  (op->content & ASM_OP_ADDRESS) ? "ASM_OP_ADDRESS" : ".");
  
  return 0;
}







