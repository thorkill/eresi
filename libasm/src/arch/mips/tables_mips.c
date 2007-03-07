/*
** 
** 
** 
**
** $Id: tables_mips.c,v 1.2 2007-03-07 16:45:34 thor Exp $
**
*/
#include <libasm.h>

int	mips_root_table[64] =
  {
    ASM_MIPS_REG,
    ASM_MIPS_,
    ASM_MIPS_J,
    ASM_MIPS_JAL,
    ASM_MIPS_BEQ,
    ASM_MIPS_BNE,
    ASM_MIPS_BLEZ,
    ASM_MIPS_BGTZ,

    ASM_MIPS_ADDI,
    ASM_MIPS_ADDIU,
    ASM_MIPS_SLTI,
    ASM_MIPS_SLTIU,
    ASM_MIPS_ANDI,
    ASM_MIPS_ORI,
    ASM_MIPS_XORI,
    ASM_MIPS_,

    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,

    ASM_MIPS_LO,
    ASM_MIPS_LHI,
    ASM_MIPS_TRAP,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_LB,
    ASM_MIPS_LH,
    ASM_MIPS_,
    ASM_MIPS_LW,
    ASM_MIPS_LBU,
    ASM_MIPS_LHU,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_SB,
    ASM_MIPS_SH,
    ASM_MIPS_,
    ASM_MIPS_SW,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_
  };

int	mips_func_table[64] = 
  {
    ASM_MIPS_SLL,
    ASM_MIPS_,
    ASM_MIPS_SRL,
    ASM_MIPS_SRA,
    ASM_MIPS_SLLV,
    ASM_MIPS_,
    ASM_MIPS_SRLV,
    ASM_MIPS_SRAV,
    
    ASM_MIPS_JR,
    ASM_MIPS_JALR,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_MFHI,
    ASM_MIPS_MTHI,
    ASM_MIPS_MFLO,
    ASM_MIPS_MTLO,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_MULT,
    ASM_MIPS_MULTU,
    ASM_MIPS_DIV,
    ASM_MIPS_DIVU,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_ADD,
    ASM_MIPS_ADDU,
    ASM_MIPS_SUB,
    ASM_MIPS_SUBU,	
    ASM_MIPS_AND,
    ASM_MIPS_OR,
    ASM_MIPS_XOR,
    ASM_MIPS_NOR,
    
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_SLT,
    ASM_MIPS_SLTU,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_,
    ASM_MIPS_

  };
