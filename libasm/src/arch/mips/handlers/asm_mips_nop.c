/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_nop(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   fprintf(stderr,"nop\n");
   return 777;
}
