/**
** @file asm_mips_break.c
** @ingroup m-instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_break(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_BREAK;
   ins->type = ASM_TYPE_STOP /* ?????? */;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_NOOP /* ASM_MIPS_OTYPE_TRAP */, ins);

   /* Exceptions: Breakpoint */

   return 4;
}
