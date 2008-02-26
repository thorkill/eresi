/**
 * @defgroup output_mips Mips Ascii output engine
 * @file output_mips.c
 * @ingroup output_mips
 *
 */
/* Fix by Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

/**
 * Return ascii representation of a mips operand
 *
 * @param ins
 * @param num
 * @parram addr
 * @return A pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_operand(asm_instr *ins,int num,unsigned int addr)
{
  /**
  static char buf[32];
   asm_operand *op;

   switch(num) {
      case 1: op = &ins->op[0]; break;
      case 2: op = &ins->op[1]; break;
      case 3: op = &ins->op[2]; break;
      default: return NULL;
   }

   switch(op->type)
   {
      case ASM_MIPS_OTYPE_REGISTER:
         if(op->imm < 10)
            sprintf(buf,"%i",op->imm);
         else
            sprintf(buf,"0x%x",op->imm);
         break;
      case ASM_MIPS_OTYPE_IMMEDIATE:
         sprintf(buf,"%s",
            config_get_data(ASM_CONFIG_MIPS_REGISTER_FLAVOUR) ==
		 ASM_CONFIG_MIPS_EXTENDED_REGISTERS) ? 
            e_mips_registers[reg].ext_mnemonic : 
            e_mips_registers[reg].mnemonic;
         break;
      default:
         return "err";
   }

   return buf;
  */
  static char bufer[32];
  asm_operand *op = &ins->op[0];
  struct s_mips_decode_reg temp;
  u_char *helper;

  bzero(&temp,sizeof(temp));
  bzero(bufer,sizeof(bufer));
  helper = (u_char*)&op->scale;

  switch(op->type) {
  
     case ASM_MIPS_OTYPE_NONE:

        snprintf(bufer,sizeof(bufer),"NOT YET!");
	break;

     case ASM_MIPS_OTYPE_REGISTER:

        mips_convert_format_r(&temp,helper);
        switch (ins->instr) {
	
	   case ASM_MIPS_CLO:
	   case ASM_MIPS_CLZ:
	   case ASM_MIPS_DCLO:
	   case ASM_MIPS_DCLZ:
	   
	      snprintf(bufer,sizeof(bufer),"%4s,%4s",e_mips_registers[temp.rd].ext_mnemonic,\
                      e_mips_registers[temp.rs].ext_mnemonic);
	      break;

           case ASM_MIPS_DSLLV:
	   case ASM_MIPS_DSRAV:
	   case ASM_MIPS_DSRLV:
	   case ASM_MIPS_SLLV:
	   case ASM_MIPS_SRAV:
	   case ASM_MIPS_SRLV:
	   
	      snprintf(bufer,sizeof(bufer),"%4s,%4s,%4s",e_mips_registers[temp.rd].ext_mnemonic,\
                      e_mips_registers[temp.rt].ext_mnemonic,e_mips_registers[temp.rs].ext_mnemonic);
	      break;

	   case ASM_MIPS_MADD:
	   case ASM_MIPS_MADDU:
	   case ASM_MIPS_MSUB:
	   case ASM_MIPS_MSUBU:
	   case ASM_MIPS_MULT:
	   case ASM_MIPS_MULTU:
	   case ASM_MIPS_TEQ:
	   case ASM_MIPS_TGE:
	   case ASM_MIPS_TGEU:
	   case ASM_MIPS_TLT:
	   case ASM_MIPS_TLTU:
	   case ASM_MIPS_TNE:
	   
	      snprintf(bufer,sizeof(bufer),"%4s,%4s",e_mips_registers[temp.rs].ext_mnemonic,\
                      e_mips_registers[temp.rt].ext_mnemonic);
	      break;

           default:

              snprintf(bufer,sizeof(bufer),"%4s,%4s,%4s",e_mips_registers[temp.rd].ext_mnemonic,\
                      e_mips_registers[temp.rs].ext_mnemonic,e_mips_registers[temp.rt].ext_mnemonic);
//              snprintf(bufer,sizeof(bufer),"%2d,%2d,%2d",temp.rd,temp.rs,temp.rt);
              break;

	}

        break;

  }
  
  return bufer;

}

/**
 * Return ascii representation of a mips instruction
 *
 * @param ins Pointer to instruction structure.
 * @param addr Virtual address of instruction.
 * @return Pointer to a static buffer or NULL on error.
 */
char *asm_mips_display_instr(asm_instr *ins,int addr)
{
  /*
    static char buf[32];

    switch(ins->nb_op)
    {
    case 1:
    sprintf(buf,"%s %s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr));
    break;
    case 2:
    sprintf(buf,"%s %s,%s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr),
    asm_mips_display_operand(ins,2,addr));
    break;
    case 3:
    sprintf(buf,"%s %s,%s,%s",
    e_mips_instrs[ins->instr],
    asm_mips_display_operand(ins,1,addr),
    asm_mips_display_operand(ins,2,addr),
    asm_mips_display_operand(ins,3,addr));
    break;
    }

    return buf;
  */
//  return ("unimplemented");
   static char buf[32];
   
   bzero(buf,sizeof(buf));
   snprintf(buf,32,"%s\t%s",e_mips_instrs[ins->instr].mnemonic,asm_mips_display_operand(ins,0x0,addr));
   return buf;
}


