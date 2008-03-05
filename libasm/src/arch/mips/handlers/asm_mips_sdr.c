/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_sdr(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_SDR;
   ins->type = ASM_TYPE_STORE;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);

   /* Exceptions: TLB Refill, TLB Invalid, TLB Modified, Bus Error, Address Error, Reserved Instruction */

   return 777;
}
