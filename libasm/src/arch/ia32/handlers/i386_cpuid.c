/*
** $Id: i386_cpuid.c,v 1.1 2007-01-26 14:18:37 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0f 0xa2
  Instruction :         CPUID
 */

int i386_cpuid(asm_instr *new, u_char *opcode, u_int len,
               asm_processor *proc) {
    new->len += 1;
    new->instr = ASM_CPUID;
  return (new->len);
}
