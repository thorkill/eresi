/*
** sparc_synthetic.c for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 08:31:32 2005 #!HATE#@!
** Last update Thu Jun 16 05:46:08 2005 #!HATE#@!
*/

#include <libasm.h>

/** \file sparc_synthetic.c */

/**
 * This function return instruction type depending on instruction.
 * \param instr Instruction id
 * \return ASM_TYPE_LOAD for ld [..], reg
 * \returns ASM_TYPE_STORE for st reg, [...]
 */

int	sparc_type_instr(int instr)
{
  switch(instr)
    {
      /** 
       * \todo Complete ASM_TYPE_LOAD instruction list. 
       */
    case ASM_SP_LD:
    case ASM_SP_LDF:
    case ASM_SP_LDUB:
      return ASM_TYPE_LOAD;
      break;
      /** 
       * \todo Complete ASM_TYPE_STORE instruction list.
       */
    case ASM_SP_ST:
    case ASM_SP_STF:
    case ASM_SP_STB:
      return ASM_TYPE_STORE;
    default:
    case ASM_SP_CALL:
      return ASM_TYPE_CALLPROC;
      break;
    case ASM_SP_JMP:
      return ASM_TYPE_IMPBRANCH;
      break;
    case ASM_SP_RET:
    case ASM_SP_RETL:
      return ASM_TYPE_RETPROC;
      return 0;
      break;
    }
  
}

/**
 * This function also convert operand type depending on instruction using 
 * integer or floating point register
 *
 */

int	sparc_ins_float(asm_instr *ins)
{ 
  if (ins->instr == ASM_SP_LDF ||
      ins->instr == ASM_SP_STF ||
      ins->instr == ASM_SP_LDDF ||
      ins->instr == ASM_SP_STDF)
    {
      ins->op1.type = ASM_SP_OTYPE_FREGISTER;
      return 1;
    }
  return 0;
}

/**
 * This function convert some instruction patterns to synthic instructions.
 * Several instructions as nop, ret, inc are specific instruction patterns.
 * \param ins Pointer to instruction to convert.
 */

void	sparc_instruction2synthetic(asm_instr *ins)
{
  if (sparc_ins_float(ins))
    return ;
  /** \todo  jmp address   |  jumpl   address, %o7		*/
  /** \todo  call reg_imm  |  jumpl   reg_imm, %o7		*/
  /** \todo  inc  reg[rd]  |  add    reg[rd], 1, reg[rd]	*/
  /** \todo  inc  const13, reg[rd]   |   add  reg[rd], const13, reg[rd]	*/
  /** \todo  inccc  reg[rd]  |  addcc    reg[rd], 1, reg[rd]	*/
  /** \todo  inccc  const13, reg[rd]   |   addcc  reg[rd], const13, reg[rd]	*/
  /** \todo  dec  reg[rd]  |  dec    reg[rd], 1, reg[rd]	*/
  /** \todo  dec  const13, reg[rd]   |   dec  reg[rd], const13, reg[rd]	*/
  /** \todo  deccc  reg[rd]  |  deccc    reg[rd], 1, reg[rd]	*/
  /** \todo  deccc  const13, reg[rd]   |   deccc  reg[rd], const13, reg[rd]	*/
  
  // tst reg[rs1]   |  orcc reg[rs], %g0 %g0
  if (ins->instr == ASM_SP_ORCC)
    {
      if (ins->op1.type == ASM_SP_OTYPE_REGISTER && ins->op1.base_reg == ASM_REG_G0 &&
	  ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == ASM_REG_G0)
	{
	  ins->instr = ASM_SP_TST;
	  ins->nb_op = 1;
	  ins->op1.base_reg = ins->op3.base_reg;
	  goto label_set_type;
	}
      
    }
  // nop           |  sethi 0, %g0
  if (ins->instr == ASM_SP_SETHI)
    {
      if (ins->op1.type == ASM_SP_OTYPE_REGISTER && ins->op1.base_reg == ASM_REG_G0 &&
	  ins->op2.type == ASM_SP_OTYPE_SETHI && ins->op2.imm == 0)
	{
	  ins->instr = ASM_SP_NOP;
	  ins->nb_op = 0;
	  goto label_set_type;
	}
    }
  if (ins->instr == ASM_SP_OR)
    {
      // clr reg[rd]     |  or   %g0, 0, reg[rd]
      if (ins->op3.type == ASM_SP_OTYPE_IMMEDIATE && ins->op3.imm == 0 &&
	  ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == ASM_REG_G0)
	{
	  ins->instr = ASM_SP_CLR;
	  ins->nb_op = 1;
	  goto label_set_type;
	}
      // mov reg_imm, reg[rd]      |    or %g0, reg_imm, reg[rd]
      if (ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == 0)
	{
	  ins->nb_op = 2;
	  ins->instr = ASM_SP_MOV;
	  ins->op2.type = ins->op3.type;
	  ins->op2.base_reg = ins->op3.base_reg;
	  ins->op2.imm = ins->op3.imm;
	  goto label_set_type;
	}
    }
  // cmp reg[rs1], reg_imm        |  subcc  reg[rs1], reg_imm, %g0
  if (ins->instr == ASM_SP_SUBCC)
    {
      // return;
      if (ins->op1.type == ASM_SP_OTYPE_REGISTER && ins->op1.base_reg == ASM_REG_G0)
	{
	  ins->nb_op = 2;
	  ins->instr = ASM_SP_CMP;
	  ins->op1.type = ins->op3.type;
	  ins->op1.base_reg = ins->op3.base_reg;
	  ins->op1.imm = ins->op3.imm;

	  ins->op3.type = ins->op2.type;
	  ins->op3.base_reg = ins->op2.base_reg;
	  ins->op3.imm = ins->op2.imm;
	  goto label_set_type;
	}
    }
  
  if (ins->instr == ASM_SP_RESTORE)
    {
      if (ins->op1.type == ASM_SP_OTYPE_REGISTER && ins->op1.base_reg == 0 &&
	  ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == 0 &&
	  ins->op3.type == ASM_SP_OTYPE_REGISTER && ins->op3.base_reg == 0)
	ins->nb_op = 0;
      return;
    }
  
  if (ins->instr == ASM_SP_JMPL)
    {
      // ret    |   jmp %i7 + 8, %g0
      if (ins->op1.type == ASM_SP_OTYPE_REGISTER && ins->op1.base_reg == ASM_REG_G0 &&
	  ins->op3.type == ASM_SP_OTYPE_IMMEDIATE && ins->op3.imm == 8)
	{
	  if (ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == ASM_REG_I7)
	    {
	      ins->instr = ASM_SP_RET;
	      ins->nb_op = 0;
	      goto label_set_type;
	    }
	  if (ins->op2.type == ASM_SP_OTYPE_REGISTER && ins->op2.base_reg == ASM_REG_O7)
	    {
	      ins->instr = ASM_SP_RETL;
	      ins->nb_op = 0;
	      goto label_set_type;
	    }
	}
	
    }
 label_set_type:
  ins->type = sparc_type_instr(ins->instr);
  return;
}



