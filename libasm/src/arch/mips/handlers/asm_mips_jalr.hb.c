/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_jalr_hb(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
//   ins->instr = ASM_MIPS_JALR;
//   asm_mips_operand_r(&ins->op[0], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);
   asm_mips_jalr(ins, buf, len, proc);

   return 4;
}
