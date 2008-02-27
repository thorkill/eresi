/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_beq(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   u_int converted = 0;

   memcpy((char *)&converted,buf,sizeof(converted));

   switch ((converted >> 16) & 0x3FF) {

      case ASM_MIPS_OTYPE_NONE:
         asm_mips_b(ins,buf,len,proc);
         break;
      default:
         ins->instr = ASM_MIPS_BEQ;
	 asm_mips_operand_i(&ins->op[0], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);
         break;
   }

   return 777;
}
