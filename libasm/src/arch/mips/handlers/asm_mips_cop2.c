/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_cop2(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_COP2;
   ins->type = ASM_TYPE_ARCH;

   /* Exceptions: Coprocessor Unusable, Reserved instruction */

   return 4;
}
