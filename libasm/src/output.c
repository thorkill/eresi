/**
 * @file output.c
 * @ingroup libasm_engine
 * $Id$
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
void	asm_resolve_immediate(asm_processor *proc, eresi_Addr val, 
			      char *buf, u_int len) 
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
char	*asm_display_instr_att(asm_instr *instr, eresi_Addr addr) 
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
    case ASM_CONTENT_FIXED: return ("fixed");
    case ASM_CONTENT_OPMOD: return ("opmod");
    case ASM_CONTENT_ADDRESS: return ("address");
    case ASM_CONTENT_CONTROL: return ("control");
    case ASM_CONTENT_DEBUG: return ("debug");
    case ASM_CONTENT_ENCODED: return ("encoded");
    case ASM_CONTENT_ENCODEDBYTE: return ("encodedbyte");
    case ASM_CONTENT_FLAGS: return ("flags");
    case ASM_CONTENT_GENERAL: return ("general");
    case ASM_CONTENT_GENERALBYTE: return ("generalbyte");
    case ASM_CONTENT_IMMEDIATE: return ("immediate");
    case ASM_CONTENT_IMMEDIATEWORD: return ("immediateword");
    case ASM_CONTENT_IMMEDIATEBYTE: return ("immediatebyte");
    case ASM_CONTENT_SHORTJUMP: return ("shortjump");
    case ASM_CONTENT_JUMP: return ("jump");
    case ASM_CONTENT_MEMORY: return ("memory");
    case ASM_CONTENT_OFFSET: return ("offset");
    case ASM_CONTENT_PMMX: return ("pmmx");
    case ASM_CONTENT_QMMX: return ("qmmx");
    case ASM_CONTENT_REGISTER: return ("register");
    case ASM_CONTENT_SEGMENT: return ("segment");
    case ASM_CONTENT_TEST: return ("test");
    case ASM_CONTENT_VSFP: return ("vsfp");
    case ASM_CONTENT_WSFP: return ("wsfp");
    case ASM_CONTENT_XSRC: return ("xsrc");
    case ASM_CONTENT_YDEST: return ("ydest");
    }
  return ("undocumented type");
}







