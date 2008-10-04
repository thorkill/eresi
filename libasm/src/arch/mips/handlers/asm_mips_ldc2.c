/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_ldc2(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_imm temp;

   ins->instr = ASM_MIPS_LDC2;
   ins->type = ASM_TYPE_LOAD | ASM_TYPE_ARCH;
   mips_convert_format_i(&temp, buf);
   ins->op[0].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].imm = temp.im;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);
   ins->op[2].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGBASE, ins);

   /* Exceptions: Coprocessor Unusable, Reserved Instruction, TLB Refill, TLB Invalid, Address Error */

   return 4;
}
