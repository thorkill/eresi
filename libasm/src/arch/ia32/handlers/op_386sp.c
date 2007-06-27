/** 
 * @file op_386sp.c
 * @ingroup handlers_ia32
 * $Id: op_386sp.c,v 1.7 2007-06-27 11:25:11 heroine Exp $
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

int     op_386sp(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc)
{ 
  vector_t      *vec;
  u_int         dim[1];
  u_char        opcode;
  int           (*fetch)(asm_instr *, u_char *, u_int, asm_processor *);
  
  if (len < 2)
    return (-1);
  if (!(vec = aspect_vector_get("disasm")))
    return (-1);
  opcode = *(buf + 1);
  dim[0] = opcode + 0x100;
  if (!(fetch = aspect_vectors_select(vec, dim)))
    return (-1);
  if (!ins->ptr_instr)
    ins->ptr_instr = buf;
  ins->len += 1;
  return (fetch(ins, buf + 1, len - 1, proc));
}

