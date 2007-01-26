/*
** $Id: op_386sp.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

#if LIBASM_USE_VECTOR == 0

/*
 * Opcode :             0x0f
 * Following byte must be fetched from i386 instruction table.
 */

int op_386sp(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
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

int     op_386sp(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc)
{ 
  vector_t      *vec;
  u_int         dim[2];
  u_char        opcode;
  int           (*fetch)(asm_instr *, u_char *, u_int, asm_processor *);
  
  
  vec = aspect_vector_get("disasm");
  dim[0] = LIBASM_VECTOR_IA32;
  opcode = *(buf + 1);
  dim[1] = opcode + 0x100;
  fetch = aspect_vectors_select(vec, dim);
  return (fetch(ins, buf + 1, len, proc));
}

#endif
