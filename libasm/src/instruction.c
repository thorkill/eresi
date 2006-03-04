/*
**
** instruction.c in
**
** Author  : <kahmm@altdev.net>
** Started : Mon Mar 15 13:58:52 2004
** Updated : Mon Mar 22 01:35:03 2004
*/

#include <libasm.h>


int	asm_instruction_get_len(asm_instr *ins, int opt, void *valptr) {
  int	*val;

  val = (int *) valptr;
  if (ins && val)
    *val = ins->len;
  return (ins->len);
}

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
      if (ins->op1.type)
	(*val)++;
      if (ins->op2.type)
	(*val)++;
      if (ins->op3.type)
	(*val)++;
    }
  return (1);
}
