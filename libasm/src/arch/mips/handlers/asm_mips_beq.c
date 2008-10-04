/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_beq(asm_instr *ins, unsigned char *buf, unsigned int len,
                  asm_processor *proc)
{
   struct s_mips_decode_imm temp;
   unsigned int converted = 0;

   memcpy((char *)&converted,buf,sizeof(converted));

   switch ((converted >> 16) & 0x3FF) {

      case ASM_MIPS_OTYPE_NONE:
         asm_mips_b(ins,buf,len,proc);
         break;
      default:
         ins->instr = ASM_MIPS_BEQ;
         ins->type = ASM_TYPE_CONDBRANCH;
         mips_convert_format_i(&temp, buf);
         ins->op[0].baser = temp.rs;
	 asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
         ins->op[1].baser = temp.rt;
         asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
         ins->op[2].imm = temp.im;
         asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_BRANCH, ins);
         break;
   }

   /* Exceptions: None */

   return 4;
}
