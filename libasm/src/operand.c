/*
** 
** operand.c in 
** 
** Author  : <kahmm@altdev.net>
** Started : Sat Mar  6 06:28:22 2004
** Updated : Wed Apr  7 19:54:38 2004
*/

#include <libasm.h>
#include <libasm-int.h>
/**
 *
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
    case 1: op = &ins->op1; break;
    case 2: op = &ins->op2; break;
    case 3: op = &ins->op3; break;
    default:
      return (-1);
    }

  memset(valptr, 0, 4);
  memcpy(valptr, &op->imm, 4);
  /*
  switch(op->type)
    {
    case ASM_OTYPE_VALUE:
      switch(op->size)
	{
	case ASM_OSIZE_BYTE: memcpy(valptr, &op->imm, 1); break;
	case ASM_OSIZE_WORD: memcpy(valptr, &op->imm, 2); break;
	case ASM_OSIZE_DWORD: memcpy(valptr, &op->imm, 4); break;
	case ASM_OSIZE_VECTOR: memcpy(valptr, &op->imm, 4); break;
	default: return (-1);
	}
      break;
    case ASM_OTYPE_JUMP:
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
 * base register field of operand
 * asm_operand must have ASM_OP_BASE defined
 * return -1 on error
 */

int     asm_operand_get_basereg(asm_instr *ins, int num, 
				int opt, void *valptr) 
{
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if (ins->op1.type && (ins->op1.content & ASM_OP_BASE))
	*val = ins->op1.base_reg;
      else
	return (-1);  
      break;
    case 2:
      if (ins->op2.type && (ins->op2.content & ASM_OP_BASE))
	*val = ins->op2.base_reg;
      else
	return (-1);  
      break;
    case 3:
      if (ins->op3.type && (ins->op3.content & ASM_OP_BASE))
	*val = ins->op3.base_reg;
      else
	return (-1);        
      break;
    default:
      return (-1);
    }
  return (1);
}

/**
 * index register field of operand
 * asm_operand must have ASM_OP_
 * return -1 on error
 */

int     asm_operand_get_indexreg(asm_instr *ins, int num, 
				 int opt, void *valptr) 
{
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if (ins->op1.type && (ins->op1.content & ASM_OP_INDEX))
	*val = ins->op1.index_reg;
      else
	return (-1);  
      break;
    case 2:
      if (ins->op2.type && (ins->op2.content & ASM_OP_INDEX))
	*val = ins->op2.index_reg;
      else
	return (-1);  
      break;
    case 3:
      if (ins->op3.type && (ins->op3.content & ASM_OP_INDEX))
	*val = ins->op3.index_reg;
      else
	return (-1);    
      break;
    default:
      return (-1);
    }
  return (1);
}

/**
 * scale field of operand
 * asn_operand must have ASM_OP_SCALE type set
 * return -1 on error
 */

int     asm_operand_get_scale(asm_instr *ins, int num,
			      int opt, void *valptr) {
  int	*val;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      if (ins->op1.type && (ins->op1.content & ASM_OP_SCALE))
	*val = ins->op1.scale;
      else
	return (-1);  
      break;
    case 2:
      if (ins->op2.type && (ins->op2.content & ASM_OP_SCALE))
	*val = ins->op2.scale;
      else
	return (-1);  
      break;
    case 3:
      if (ins->op3.type && (ins->op3.content & ASM_OP_SCALE))
	*val = ins->op3.scale;
      else
	return (-1);        
      break;
    default:
      return (-1);
    }
  return (1);
}

/*
  
 */

int    asm_operand_set_basereg(asm_instr *ins, int num, 
			       int opt, void *valptr) 
{
  asm_modrm	*modrm;
  asm_operand	*op;
  int		*val;
  
  if (!op->ptr) {
    fprintf(stderr, "no pointer available");
  }
  else
    {
      val = (int *) valptr;
      modrm = (asm_modrm *) op->ptr;
    switch(op->type)
      {
      case ASM_OTYPE_OPMOD:
	modrm->m = *val;
	break;
      case ASM_OTYPE_CONTROL:
	modrm->m = *val;
	break;
      case ASM_OTYPE_DEBUG:
	modrm->m = *val;
	break;
      case ASM_OTYPE_ENCODED:
	modrm->r = *val;
	break;
      case ASM_OTYPE_GENERAL:
	modrm->r = *val;
	break;
      case ASM_OTYPE_REGISTER:
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
  
int    asm_operand_set_indexreg(asm_instr *ins, int num, 
				int opt, void *valptr) {
  int			*val;
  struct s_sidbyte	*sidbyte;
  asm_operand		*op;

  if (op->content & ASM_OP_INDEX) {
    sidbyte = (struct s_sidbyte *) (op->ptr + 1);
    val = (int *) valptr;
    switch(op->type) {
    case ASM_OTYPE_ENCODED:
    case ASM_OTYPE_MEMORY:
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
     
int    asm_operand_set_scale(asm_instr *ins, int num,
			     int opt, void *valptr) {
  int			*val;
  struct s_sidbyte      *sidbyte;
  asm_operand		*op;
  
  val = (int *) valptr;
  if (op && op->type & ASM_OP_SCALE) {
    
    if (op->type & ASM_OP_FPU)
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

int	asm_operand_get_type(asm_instr *ins, int num, int opt, void *valptr) {
  int	*val;
  
  if ((val = valptr))
    switch(num)
      {
      case 1:
	return (*val = ins->op1.type);
      case 2:
	return (*val = ins->op2.type);
      case 3:
	return (*val = ins->op3.type);
      }
  return (-1);
}


int	asm_operand_get_size(asm_instr *ins, int num, int opt, void *valptr) {
  int	*val, to_ret;
  
  val = (int *) valptr;
  switch(num)
    {
    case 1:
      *val = ins->op1.size;
      to_ret = 1;
      break;
    case 2:
      *val = ins->op2.size;
      to_ret = 1;
      break;
    case 3:
      *val = ins->op3.size;
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
      return (ins->op1.len);
    case 2:
      return (ins->op2.len);
    case 3:
      return (ins->op3.len);
    default:
      return (0);
    }
}

/**
 *
 *
 */


int	asm_operand_get_count(asm_instr *ins, int num, int opt, void *valptr) {
  
  num = 0;
  if (ins->op1.content)
    num++;
  if (ins->op2.content)
    num++;
  if (ins->op3.content)
    num++;
  return (num);
}



/*

int	asm_operand_get_type(asm_instr *ins, int num, int opt, void *valptr)
{
  int     *typeptr;
  
  typeptr = valptr;
  typeptr & ASM_OTYPE_MASK;
  
  }

  int	asm_operand_set_type(asm_instr *ins, int num, int opt, void *valptr)
  {
  int *typeptr;
  
  
  typeptr = valptr;
  
}


int	asm_operand_get_size(asm_instr *ins, int num, int opt, void *valptr)
{
  if (valptr)
    switch(num)
      {
      case 1:
	*valptr = ins->op1.size;
	return (1);
      case 2:
	*valptr = ins->op2.size;
	return (1);
      case 3:
	*valptr = ins->op2.size;
	return (1);
      }
  else
    return (-1);
}


int	asm_operand_set_size(asm_instr *ins, int num, int opt, void *valptr)
{
  if (valptr)
    switch(num)
      {
  
  
      }
  else
    
}

*/




