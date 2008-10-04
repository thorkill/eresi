/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

/* TGE rs,rt */

int asm_mips_tge(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_trap temp;

   ins->instr = ASM_MIPS_TGE;
   ins->type = ASM_TYPE_ARCH | ASM_TYPE_COMPARISON | ASM_TYPE_INT;
   mips_convert_format_t(&temp,buf);
   ins->op[0].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exceptions: Trap */

   return 4;
}
