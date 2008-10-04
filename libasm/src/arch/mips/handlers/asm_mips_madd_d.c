/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_madd_d(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_cop1x temp;

   ins->instr = ASM_MIPS_MADD_D;
   ins->type = ASM_TYPE_ARITH | ASM_TYPE_ARCH;
   mips_convert_format_cop1x(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.f2;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].regset = ASM_MIPS_REG_FPU;
   ins->op[1].baser = temp.bs;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].regset = ASM_MIPS_REG_FPU;
   ins->op[2].baser = temp.f2;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[3].regset = ASM_MIPS_REG_FPU;
   ins->op[3].baser = temp.in;
   asm_mips_operand_fetch(&ins->op[3], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exception: Reserved Instruction, Coprocessor Unusable */

   return 4;
}
