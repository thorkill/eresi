/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_wsbh(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_WSBH;
   return 4;
}
