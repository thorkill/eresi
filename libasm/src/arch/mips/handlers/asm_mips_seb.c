/**
** @file asm_mips_seb.c
** @ingroup m-instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_seb(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_SEB;
   return 4;
}
