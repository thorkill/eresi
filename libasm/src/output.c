/*
** $Id: output.c,v 1.2 2007-02-21 03:43:51 heroine Exp $
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
















