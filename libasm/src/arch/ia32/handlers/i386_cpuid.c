/*
** $Id: i386_cpuid.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0f 0xa2
  Instruction :         CPUID
 */

int i386_cpuid(asm_instr *new, unsigned char *opcode, unsigned int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_CPUID;
  return (new->len);
}
