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
  static char bufer[32];
  asm_operand *op = &ins->op[0];
  struct s_mips_decode_reg temp;
  struct s_mips_decode_imm temp2;
  u_char *helper;

  bzero(&temp,sizeof(temp));
  bzero(&temp2,sizeof(temp2));
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

     case ASM_MIPS_OTYPE_IMMEDIATE:

        mips_convert_format_i(&temp2,helper);
	switch (ins->instr) {
	
	   case ASM_MIPS_B:
           case ASM_MIPS_BAL:

              snprintf(bufer,sizeof(bufer),"0x%x",temp2.im);
	      break;

	   case ASM_MIPS_BEQ:
           case ASM_MIPS_BEQL:
	   case ASM_MIPS_BNE:
	   case ASM_MIPS_BNEL:

              snprintf(bufer,sizeof(bufer),"%4s,%4s,0x%x",e_mips_registers[temp2.rs].ext_mnemonic,\
                      e_mips_registers[temp2.rt].ext_mnemonic,temp2.im);
	      break;

           case ASM_MIPS_BGEZ:
	   case ASM_MIPS_BGEZAL:
	   case ASM_MIPS_BGEZALL:
	   case ASM_MIPS_BGEZL:
	   case ASM_MIPS_BGTZ:
	   case ASM_MIPS_BGTZL:
	   case ASM_MIPS_BLEZ:
	   case ASM_MIPS_BLEZL:
	   case ASM_MIPS_BLTZ:
	   case ASM_MIPS_BLTZAL:
	   case ASM_MIPS_BLTZALL:
	   case ASM_MIPS_BLTZL:
	   case ASM_MIPS_TEQI:
	   case ASM_MIPS_TGEI:
	   case ASM_MIPS_TGEIU:
	   case ASM_MIPS_TLTI:
	   case ASM_MIPS_TLTIU:
	   case ASM_MIPS_TNEI:

              snprintf(bufer,sizeof(bufer),"%4s,0x%x",e_mips_registers[temp2.rs].ext_mnemonic,temp2.im);
	      break;

/*
           case ASM_MIPS_CACHE:

              snprintf(bufer,sizeof(bufer),"%4s,0x%x",e_mips_registers[temp.rs].ext_mnemonic,temp.im);
	      break;
*/

           case ASM_MIPS_LB:
	   case ASM_MIPS_LBU:
	   case ASM_MIPS_LD:
	   case ASM_MIPS_LDC2:
	   case ASM_MIPS_LDL:
	   case ASM_MIPS_LDR:
	   case ASM_MIPS_LH:
	   case ASM_MIPS_LHU:
	   case ASM_MIPS_LL:
	   case ASM_MIPS_LLD:
           case ASM_MIPS_LW:
	   case ASM_MIPS_LWC2:
	   case ASM_MIPS_LWL:
	   case ASM_MIPS_LWR:
	   case ASM_MIPS_LWU:
	   case ASM_MIPS_PREF:
	   case ASM_MIPS_SB:
	   case ASM_MIPS_SC:
	   case ASM_MIPS_SCD:
	   case ASM_MIPS_SD:
	   case ASM_MIPS_SDC2:
	   case ASM_MIPS_SDL:
	   case ASM_MIPS_SDR:
	   case ASM_MIPS_SH:
	   case ASM_MIPS_SW:
	   case ASM_MIPS_SWC2:
	   case ASM_MIPS_SWL:
	   case ASM_MIPS_SWR:

              snprintf(bufer,sizeof(bufer),"%4s,0x%x(%4s)",e_mips_registers[temp2.rt].ext_mnemonic,\
	              temp2.im,e_mips_registers[temp2.rs].ext_mnemonic);
	      break;

           case ASM_MIPS_LUI:

              snprintf(bufer,sizeof(bufer),"%4s,0x%x",e_mips_registers[temp2.rt].ext_mnemonic,temp2.im);
	      break;

	   default:

              snprintf(bufer,sizeof(bufer),"%4s,%4s,0x%x",e_mips_registers[temp2.rt].ext_mnemonic,\
                      e_mips_registers[temp2.rs].ext_mnemonic,temp2.im);
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
   static char buf[32];
   
   bzero(buf,sizeof(buf));
   snprintf(buf,32,"%s %s",e_mips_instrs[ins->instr].mnemonic,asm_mips_display_operand(ins,0x0,addr));
   return buf;
}


