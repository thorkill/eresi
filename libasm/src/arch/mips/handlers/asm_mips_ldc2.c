/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_ldc2(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_LDC2;
   ins->type = ASM_TYPE_LOAD | ASM_TYPE_ARCH;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);

   /* Exceptions: Coprocessor Unusable, Reserved Instruction, TLB Refill, TLB Invalid, Address Error */

   return 777;
}
