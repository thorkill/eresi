/**
 * @file output.c
 * @ingroup libasm_engine
 * $Id: output.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
 *  
 * Author  : <sk at devhell dot org>
 * Started : Xxx Xxx xx xx:xx:xx 2002
 * Updated : Thu Mar 11 00:40:31 2004
 * @brief ASCII Output wrappers.
 */
#include <libasm.h>
#include <libasm-int.h>
/**
 * @brief Wrapper to internal resolving handler for immediate values.
 * @param proc Pointer to processor structure
 * @param val Value to resolve
 * @param buf Pointer to store string
 * @param len Length of string
 */
void	asm_resolve_immediate(asm_processor *proc, unsigned int val, 
			      char *buf, unsigned int len) 
{
  LIBASM_PROFILE_FIN();
  proc->resolve_immediate(proc->resolve_data, val, buf, len);  
  LIBASM_PROFILE_VOUT();
}


/**
 * @brief Wrapper to internal Ascii output handler.
 * @param instr Pointer to instruction structure
 * @param addr Virtual address of instruction
 * @return Pointer to a static buffer containing current instruction string
 */
char	*asm_display_instr_att(asm_instr *instr, 
			       int addr) 
{
  char	*to_ret;
  LIBASM_PROFILE_FIN();
  to_ret = instr->proc->display_handle(instr, addr);
  LIBASM_PROFILE_ROUT(to_ret);
}


/**
 * merged from mjollnir
 * @brief Debug dump to stderr of an operand
 * @param op Pointer to operand structure.
 * @return Always returns 0.
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

/**
 * @brief Return a string describing otype
 * @param type Instruction type
 * @return A pointer to a static string
 */

char	*asm_operand_type_string(int type)
{
  switch (type)
    {
    case ASM_OTYPE_FIXED: return ("fixed");
    case ASM_OTYPE_OPMOD: return ("opmod");
    case ASM_OTYPE_ADDRESS: return ("address");
    case ASM_OTYPE_CONTROL: return ("control");
    case ASM_OTYPE_DEBUG: return ("debug");
    case ASM_OTYPE_ENCODED: return ("encoded");
    case ASM_OTYPE_ENCODEDBYTE: return ("encodedbyte");
    case ASM_OTYPE_FLAGS: return ("flags");
    case ASM_OTYPE_GENERAL: return ("general");
    case ASM_OTYPE_GENERALBYTE: return ("generalbyte");
    case ASM_OTYPE_IMMEDIATE: return ("immediate");
    case ASM_OTYPE_IMMEDIATEWORD: return ("immediateword");
    case ASM_OTYPE_IMMEDIATEBYTE: return ("immediatebyte");
    case ASM_OTYPE_SHORTJUMP: return ("shortjump");
    case ASM_OTYPE_JUMP: return ("jump");
    case ASM_OTYPE_MEMORY: return ("memory");
    case ASM_OTYPE_OFFSET: return ("offset");
    case ASM_OTYPE_PMMX: return ("pmmx");
    case ASM_OTYPE_QMMX: return ("qmmx");
    case ASM_OTYPE_REGISTER: return ("register");
    case ASM_OTYPE_SEGMENT: return ("segment");
    case ASM_OTYPE_TEST: return ("test");
    case ASM_OTYPE_VSFP: return ("vsfp");
    case ASM_OTYPE_WSFP: return ("wsfp");
    case ASM_OTYPE_XSRC: return ("xsrc");
    case ASM_OTYPE_YDEST: return ("ydest");
    }
  return ("undocumented type");
}







