/**
 * @file op_386sp.c
 * @ingroup handlers_ia32
 * $Id: op_386sp.c,v 1.8 2007/10/14 00:01:41 heroine Exp $
 * ChangeLog:
 * 2007-05-30	Fixed a bug in fetching. The vector used was the previously defined.
 *		strauss set up a new disasm vector and didn't know about it which
 *		was called here.
 *		Filled instruction opcode pointer.
 *		Removed the old unused handler.
 *		Added minimal error management.
 */
#include <libasm.h>
#include <libasm-int.h>


/**
 * This is the handler for 2 bytes instruction, opcode 0x0f
 * The second byte is used to fetch a new handler in the vector
 * starting at offset 0x100.
 * @param ins Pointer to instruction structure.
 * @param buf Pointer to data to disassemble.
 * @param len Length of dat to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction or -1 error.
 */

int     op_386sp(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc)
{
  int        opcode;
  int           (*fetch)(asm_instr *, unsigned char *, unsigned int, asm_processor *);

  // XXX: Use asm_set_error to set error code to LIBASM_ERROR_TOOSHORT
  if (len < 2)
    return (-1);
  
  opcode = *(buf + 1);
  opcode += 0x100;
  fetch = asm_opcode_fetch(LIBASM_VECTOR_OPCODE_IA32, opcode);
  if (!fetch)
    return (-1);
  if (!ins->ptr_instr)
    ins->ptr_instr = buf;
  ins->len += 1;
  return (fetch(ins, buf + 1, len - 1, proc));
}

