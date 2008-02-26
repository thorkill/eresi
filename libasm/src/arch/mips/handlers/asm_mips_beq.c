/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include "libasm.h"

int asm_mips_beq(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   u_int i, converted = 0;
   u_char *for_help;

   if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
      memcpy((char *)&converted,buf,sizeof(converted));
   } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     for_help = (u_char*)&converted;
     for(i=0;i<4;i++)
        *(for_help + i) = *(buf + 3 - i);
   } else {
     printf("[BEQ] Where am I ?!?!?!\n");
     exit(-1);
   }

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
