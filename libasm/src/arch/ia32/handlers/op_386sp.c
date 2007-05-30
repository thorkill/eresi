/** 
 * @file op_386sp.c
 * $Id: op_386sp.c,v 1.5 2007-05-30 00:15:41 heroine Exp $
 * ChangeLog:
 * 2007-05-30	Fixed a bug in fetching. The vector used was the previously defined.
 *		strauss set up a new disasm vector and didn't know about it which 
 *		was called here.
 *
 */
#include <libasm.h>
#include <libasm-int.h>

#if LIBASM_USE_VECTOR == 0

/*
 * Opcode :             0x0f
 * Following byte must be fetched from i386 instruction table.
 */

int op_386sp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  asm_i386_processor    *i386p;
  
  /* this is a bridge to i386 instructions, pentium, ... instructions */
  new->ptr_instr = opcode;
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->cisc_i386[*(opcode + 1)]) {
    new->len += 1;
    return (i386p->internals->cisc_i386[*(opcode + 1)](new, opcode + 1, 
                                                      len - 1, proc));  
  }
  else
    return (0);
}

#else

/**
 * This is the handler which will find in the vector for extended instructions
 * which are encoded on 2 bytes.
 * The second byte
 * @param
 */

int     op_386sp(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc)
{ 
  vector_t      *vec;
  u_int         dim[1]; // dim[4];
  u_char        opcode;
  int           (*fetch)(asm_instr *, u_char *, u_int, asm_processor *);
  
  vec = aspect_vector_get("disasm");
  opcode = *(buf + 1);
  dim[0] = opcode + 0x100;
  /*
  dim[0] = LIBASM_VECTOR_IA32;
  dim[1] = opcode + 0x100;
  dim[2] = 0;
  dim[3] = 0;
  */
  fetch = aspect_vectors_select(vec, dim);
  ins->len += 1;
  return (fetch(ins, buf + 1, len - 1, proc));
}

#endif
