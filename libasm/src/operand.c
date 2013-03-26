/**
 * @file operand.c
 * @ingroup operands
 * $Id$
 * operand.c in 
 * 
 * Author  : <kahmm@altdev.net>
 * Started : Sat Mar  6 06:28:22 2004
 * Updated : Wed Apr  7 19:54:38 2004
 * @brief Contains operand manipulation functions.
*/

#include <libasm.h>
#include <libasm-int.h>

/**
 * Return immediate part of the operand
 * @param ins Pointer to instruction
 * @param num Number of the operand. (SOURCE/DEST/OPT)
 * @param opt Optionnal. May be the virtual address of instruction
 * @param valptr Reference to store immediate content
 */
int     asm_operand_get_immediate(asm_instr *ins, int num,
				  int opt, void *valptr) 
{  

  asm_operand	*op;
  /*
  char		*c;
  short		*s;
  int		*i;
  */

  switch(num) 
    {
    case 1: op = &ins->op[0]; break;
    case 2: op = &ins->op[1]; break;
    case 3: op = &ins->op[2]; break;
    default:
      return (-1);
    }
  
  memset(valptr, 0, 4);
  memcpy(valptr, &op->imm, 4);

  /**
   * This is pseudo dead code
   * it was about 
   *
   */
  /*
  switch(op->type)
    {
    case ASM_CONTENT_VALUE:
      switch(op->size)
	{
	case ASM_OSIZE_BYTE: memcpy(valptr, &op->imm, 1); break;
	case ASM_OSIZE_WORD: memcpy(valptr, &op->imm, 2); break;
	case ASM_OSIZE_DWORD: memcpy(valptr, &op->imm, 4); break;
	case ASM_OSIZE_VECTOR: memcpy(valptr, &op->imm, 4); break;
	default: return (-1);
	}
      break;
    case ASM_CONTENT_JUMP:
      switch(op->size)
	{
	case ASM_OSIZE_BYTE: c = op->ptr; opt = opt + *c; break;
	case ASM_OSIZE_WORD: s = (short *)op->ptr; opt += *s; break;
	case ASM_OSIZE_DWORD: i = (int *) op->ptr; opt += *i; break;
	case ASM_OSIZE_VECTOR: i = (int *) op->ptr; opt += *i; break;
	}
      memcpy(valptr, &opt, 4);
      break;
    }
  */  
  return (1);
}


/**
 * Get base register of operand
 * asm_operand must have ASM_OP_BASE defined
 * @param ins Pointer to instruction
 * @param num Number of the operand
 * @param opt Optionnal parameter, may be virtual address
 * @param valptr Reference to store the content of the register
 * @return -1 on error, 
 */

int     asm_operand_get_basereg(asm_instr *ins, int num, 
				int opt, void *valptr) 
{
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if (ins->op[0].type == ASM_OPTYPE_REG ||
            ((ins->op[0].type == ASM_OPTYPE_MEM) && (ins->op[0].memtype & ASM_OP_BASE)))
        *val = ins->op[0].baser;
      else
        return (-1);  
      break;
    case 2:
      if (ins->op[1].type == ASM_OPTYPE_REG ||
            ((ins->op[1].type == ASM_OPTYPE_MEM) && (ins->op[1].memtype & ASM_OP_BASE)))
        *val = ins->op[1].baser;
      else
        return (-1);
      break;
    case 3:
      if (ins->op[2].type == ASM_OPTYPE_REG ||
            ((ins->op[2].type == ASM_OPTYPE_MEM) && (ins->op[2].memtype & ASM_OP_BASE)))
        *val = ins->op[2].baser;
      else
        return (-1);        
      break;
    default:
      return (-1);
    }
  return (1);
}

/**
 * Get index register field of operand
 * asm_operand must have ASM_OP_INDEX
 * @return -1 on errorreturn -1 on error
 */

int     asm_operand_get_indexreg(asm_instr *ins, int num, 
				 int opt, void *valptr) 
{
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if ((ins->op[0].type == ASM_OPTYPE_MEM) && (ins->op[0].memtype & ASM_OP_INDEX))
        *val = ins->op[0].indexr;
      else
        return (-1);  
      break;
    case 2:
      if ((ins->op[1].type == ASM_OPTYPE_MEM) && (ins->op[1].memtype & ASM_OP_INDEX))
        *val = ins->op[1].indexr;
      else
        return (-1);  
      break;
    case 3:
      if ((ins->op[2].type == ASM_OPTYPE_MEM) && (ins->op[2].memtype & ASM_OP_INDEX))
        *val = ins->op[2].indexr;
      else
        return (-1);    
      break;
    default:
      return (-1);
    }
  return (1);
}

/**
 * Get scale field of operand
 * asn_operand must have ASM_OP_SCALE type set
 * @return -1 on error, 1 on success
 */

int     asm_operand_get_scale(asm_instr *ins, int num,
			      int opt, void *valptr) {
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if (((ins->op[0].type == ASM_OPTYPE_REG) && (ins->op[0].content == ASM_CONTENT_FPU_SCALED)) ||
          ((ins->op[0].type == ASM_OPTYPE_MEM) && (ins->op[0].memtype & ASM_OP_SCALE)))
        *val = ins->op[0].scale;
      else
	return (-1);  
      break;
    case 2:
      if (((ins->op[1].type == ASM_OPTYPE_REG) && (ins->op[1].content == ASM_CONTENT_FPU_SCALED)) ||
          ((ins->op[1].type == ASM_OPTYPE_MEM) && (ins->op[1].memtype & ASM_OP_SCALE)))
        *val = ins->op[1].scale;
      else
	return (-1);  
      break;
    case 3:
      if (((ins->op[2].type == ASM_OPTYPE_REG) && (ins->op[2].content == ASM_CONTENT_FPU_SCALED)) ||
          ((ins->op[2].type == ASM_OPTYPE_MEM) && (ins->op[2].memtype & ASM_OP_SCALE)))
        *val = ins->op[2].scale;
      else
	return (-1);        
      break;
    default:
      return (-1);
    }
  return (1);
}

/**
 * Set base register of operand 
 * @param ins
 * @param num
 * @param opt
 * @param valptr
 * @return -1 on error, 1 on success
 */

int    asm_operand_set_basereg(asm_instr *ins, int num, 
			       int opt, void *valptr) 
{
  asm_modrm	*modrm;
  asm_operand	*op;
  int *val;
  
  return (-1);
  if (!op->ptr) {
    fprintf(stderr, "no pointer available");
  }
  else
    {
      val = (int *) valptr;
      modrm = (asm_modrm *) op->ptr;
    switch(op->type)
      {
      case ASM_CONTENT_OPMOD:
        modrm->m = *val;
        break;
      case ASM_CONTENT_CONTROL:
        modrm->m = *val;
        break;
      case ASM_CONTENT_DEBUG:
        modrm->m = *val;
        break;
      case ASM_CONTENT_ENCODED:
        modrm->r = *val;
        break;
      case ASM_CONTENT_GENERAL:
        modrm->r = *val;
        break;
      case ASM_CONTENT_REGISTER:
        modrm->m = *val;
        break;	
      default:
        return (0);
      }
    /*
    if ((op->type & ASM_OP_BASE) && 
	((ASM_OP_INDEX & op->type) || (ASM_OP_SCALE))) {

    */
    }
  
  return (1);
}


/**
 * Set index register of operand 
 * @param ins
 * @param num
 * @param opt
 * @param valptr
 * @return -1 on error, 1 on success
 */

int    asm_operand_set_indexreg(asm_instr *ins, int num, 
				int opt, void *valptr) {
  int			*val;
  struct s_sidbyte	*sidbyte;
  asm_operand		*op;

  return (-1);
  if ((op->type == ASM_OPTYPE_MEM) && (op->memtype & ASM_OP_INDEX)) {
    sidbyte = (struct s_sidbyte *) (op->ptr + 1);
    val = (int *) valptr;
    switch(op->content) {
      case ASM_CONTENT_ENCODED:
      case ASM_CONTENT_MEMORY:
        if (!op->ptr)
          fprintf(stderr, "ptr field NULL, cannot set index reg\n");
        else
          sidbyte->index = *val;
        break;
      default:
        fprintf(stderr, "unsupported operand type to set index register\n");
    }
  }

  return (-1);
}

/**
 * Set scale field of operand 
 * @param ins
 * @param num
 * @param opt
 * @param valptr
 * @return -1 on error, 1 on success
 */
int    asm_operand_set_scale(asm_instr *ins, int num,
			     int opt, void *valptr) {
  int			*val;
  struct s_sidbyte      *sidbyte;
  asm_operand		*op;
  
  return (-1);
  op = &ins->op[0];
  val = (int *) valptr;

  if (((op->type == ASM_OPTYPE_REG) && (op->content == ASM_CONTENT_FPU_SCALED)) ||
      ((op->type == ASM_OPTYPE_MEM) && (op->memtype & ASM_OP_SCALE))) {
    
    if (op->content == ASM_CONTENT_FPU_SCALED)
      sidbyte->sid = *val;
    else
      switch(*val) {
      case 1:
        sidbyte->sid = 0; break;
      case 2:
        sidbyte->sid = 1; break;
      case 4:
        sidbyte->sid = 2; break;
      case 8:
        sidbyte->sid = 3; break;
      default:
	return (0);
      } 
  }
  return (1);
}

/**
 * 
 * 
 *
 */

int	asm_operand_get_type(asm_instr *ins, int num, int opt, void *valptr) {
  int	*val;
  
  if ((val = valptr))
    switch(num)
      {
      case 1:
        return (*val = ins->op[0].type);
      case 2:
        return (*val = ins->op[1].type);
      case 3:
        return (*val = ins->op[2].type);
      }
  return (-1);
}


int	asm_operand_get_size(asm_instr *ins, int num, int opt, void *valptr) {
  int	*val, to_ret;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      *val = ins->op[0].size;
      to_ret = 1;
      break;
    case 2:
      *val = ins->op[1].size;
      to_ret = 1;
      break;
    case 3:
      *val = ins->op[2].size;
      to_ret = 1;
      break;
    default:
      to_ret = -1;
    }
  return (to_ret);
}

int	asm_operand_get_len(asm_instr *ins, int num, int opt, void *valptr) {
  switch(num)
    {
    case 1:
      return (ins->op[0].len);
    case 2:
      return (ins->op[1].len);
    case 3:
      return (ins->op[2].len);
    default:
      return (0);
    }
}

/**
 * Return number of operands
 * @param ins Pointer to instruction *
 */
int	asm_operand_get_count(asm_instr *ins, int num, int opt, void *valptr) 
{
  if (!ins)
    return (-1);
  num = 0;
  if (ins->op[0].content)
    num++;
  if (ins->op[1].content)
    num++;
  if (ins->op[2].content)
    num++;
  return (num);
}


/**
 * Return wether operand if a reference or not.
 * @param op Pointer to operand structure.
 * @return True if operand is a reference, or False.
 */
int	asm_operand_is_reference(asm_operand *op)
{
  return((op->type == ASM_OPTYPE_MEM) && (op->memtype & ASM_OP_REFERENCE));
}

