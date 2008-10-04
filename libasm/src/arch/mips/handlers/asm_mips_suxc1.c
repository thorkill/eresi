/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_suxc1(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_cop1x temp;

   ins->instr = ASM_MIPS_SUXC1;
   ins->type = ASM_TYPE_STORE | ASM_TYPE_ARCH;
   mips_convert_format_cop1x(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.f1;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].baser = temp.in;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].baser = temp.bs;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGBASE, ins);

   /* Exception: TLB Refill, TLB Invalid, TLB Modified, Reserved Instruction, Coprocessor Unusable */

   return 4;
}
