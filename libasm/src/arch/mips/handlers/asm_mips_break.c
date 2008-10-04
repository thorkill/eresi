/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_break(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_BREAK;
   ins->type = ASM_TYPE_STOP /* ?????? */;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_NOOP /* ASM_MIPS_OTYPE_TRAP */, ins);

   /* Exceptions: Breakpoint */

   return 4;
}
