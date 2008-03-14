/**
 *
 * @file instruction.c
 * @ingroup libasm_engine 
 * Author  : <kahmm@altdev.net>
 * Started : Mon Mar 15 13:58:52 2004
 * Updated : Mon Mar 22 01:35:03 2004
 *
 * $Id: instruction.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
 *
 */

#include <libasm.h>

/**
 * Return instruction length.
 * @param ins Pointer to instruction structure.
 * @param opt Optional parameter, not used.
 * @param valptr Optional parameter, not used.
 * @return Instruction length
 */

int	asm_instruction_get_len(asm_instr *ins, int opt, void *valptr) {
  int	*val;
  
  val = (int *) valptr;
  if (ins && val)
    *val = ins->len;
  return (ins->len);
}

/**
 * @param
 * @param opt Optional parameter. Not used. 
 * @param valptr
 * @return Returns 0.
 */

int	asm_instruction_get_mnemonic(asm_instr *ins, int opt, void *valptr) 
{
  char	**ptr;
  
  if (valptr)
    {
      ptr = valptr;
      *ptr = ins->proc->instr_table[ins->instr];
    }
  return (0);
}

/**
 * @brief Get instruction type.
 * @return Returns 1 on success, 0 on error.
 */

int	asm_instruction_get_type(asm_instr *ins, int opt, void *valptr)
{
  int	*val;
  
  if (valptr)
    {
      val = valptr;
      *val = ins->type;
    }
  return (1);
}

/**
 * @brief Get instruction prefixes
 * @param ins Pointer to instruction.
 * @param opt Optional parameter.
 * @param valptr Pointer to store prefix (bitflag stored on an integer)
 * @return Returns 1.
 */

int	asm_instruction_get_prefix(asm_instr *ins, int opt, void *valptr)
{
  int	*val;
  
  if (valptr)
    {
      val = valptr;
      *val = ins->prefix;
    }
  return (1);
}

/**
 * get number of operand of instruction
 *  
 */

int	asm_instruction_get_nbop(asm_instr *ins, int opt, void *valptr)
{
  int	*val;

  if (valptr)
    {
      val = valptr;
      *val = 0;
      if (ins->op[0].type)
	(*val)++;
      if (ins->op[1].type)
	(*val)++;
      if (ins->op[2].type)
	(*val)++;
    }
  return (1);
}

/**
 * @brief Return instruction length
 * @param instr Pointer to instruction.
 */
int	asm_instruction_get_length(asm_instr *instr)
{
  if (instr)
    return (instr->len);
  else
    return (-1);
}
