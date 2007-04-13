/*
** Latest edition Author : $Author: heroine $
** $Id: generic.c,v 1.7 2007-04-13 06:56:34 heroine Exp $
** Started : Wed Jul 24 18:45:15 2002
** Updated : Sat Mar 20 05:26:26 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @file generic.c
 *
 *
 */

/** Fetch instruction using asm_processor handler
 * func asm_read_instr
 * fetch instruction stored in buffer of max length len
 * according to specified processor.
 *\param i instruction to fill.
 *\param buf pointer to opcode to disassemble
 *\param len maximum length of opcode to disassemble
 *\return -1 on error or instruction fetched length
 */

int	asm_read_instr(asm_instr *i, u_char *buf, u_int len, asm_processor *proc) {
  memset(i, 0, sizeof (asm_instr));
  i->proc = proc;
  return (proc->fetch(i, buf, len, proc));
}

/**
 *
 *
*/

int	asm_write_instr(asm_instr *instr, u_char *buf, u_int len) {
  /*
  if (instr->op1)
    asm_write_operand(instr->op1);
  if (instr->op2)
    asm_write_operand(instr->op1);
  if (instr->op3)
    asm_write_operand(instr->op1)
  fprintf(stderr, "asm_write unimplemented\n");
  */
  return (0);
}

/**
 * Return instruction length
 * @param ins
 */


int	asm_instr_len(asm_instr *i) {
  return (asm_instruction_get_len(i, 0, 0));
}

/**
 * Return 2 power val
 * @param val The level of power to raise 2.
 * @return Return the computed value.
 */
int     asm_int_pow2(int val) {
  return (val ? (2 * asm_int_pow2(val - 1)) :  1);
}


/*
int	asm_int_pow2(int i) {
  return (i ? (2 * asm_int_pow2(i - 1)) :  1);
}
*/

/**
 * Set the resolving handler to display addresses.
 * @param proc Pointer to processor structure.
 * @param fcn The resolving handler to use
 */

void	asm_set_resolve_handler(asm_processor *proc, 
			        void (*fcn)(void *, u_int, char *, u_int), void *d) {
  proc->resolve_immediate = fcn;
  proc->resolve_data = d;
}

/*
** returns 1 if OPLEN is not activated
*/
/**
 * returns 1 if OPLEN is not activated
 * This is related only to ia32 architecture.
 * @param proc The pointer to the processor structure. 
 * @return 1 if OPLEN is activated, 0 if not.
 */

int     asm_proc_is_protected(asm_processor *proc) {
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->mode == INTEL_PROT)
    return (1);
  return (0);
}


/**
 * returns the value of the processor's current
 * operand length
 */
/**
 * returns the value of the processor's current operand length
 * This is related only to ia32 architecture.
 * @param proc Pointer to the processor structure.
 * @return 1 if opsize is set, 0 if not.
 */

int     asm_proc_opsize(asm_processor *proc) {
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->opsize)
    return (1);
  return (0);
}

/**
 * returns the value of the processor's current
 * address size
 */
/**
 * returns the value of the processor's current address size
 * @param proc Pointer to the processor structure
 * @return 1 if addsize is set, 0 if not 
 */

int     asm_proc_addsize(asm_processor *proc) {
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->addsize)
    return (1);
  return (0);
}

/*
 * returns 4 or 2 depending on eventual OPLEN prefix
 */
/**
 * Returns vector current size depending on oplen prefix
 * This is related to ia32 architecture only
 * @param proc Pointer to the processor structure.
 * @return 4 or 2 depending on
 */

int     asm_proc_vector_len(asm_processor *proc) {
  if (asm_proc_opsize(proc))
    return (2);
  else
    return (4);
}

/**
 * Return vector size depending on prefix
 */
/**
 * Return vector size depending on prefix
 * This is ia32 related.
 * @param proc Pointer to the processor structure
 * @return Return vector size.
 */

int	asm_proc_vector_size(asm_processor *proc) {
  if (asm_proc_opsize(proc))
    return (2);
  else
    return (4);
}

/*
 * set the immediate part of the operand if present
 * return number of bytes written
 */
/**
 * Set the immediate part of the operand if present
 * @param ins Pointer to instruction
 * @param num Number of the operand to set.
 * @param opt Optionnal. may be used to specify current virtual address.
 * @param valptr Pointer to the new value
 * @return number of bytes written
 */

int    asm_operand_set_immediate(asm_instr *ins, int num, 
				 int opt, void *valptr) 
{
  int		*value;
  int		off;
  int		len;
  asm_operand	*op;

  value = (int *) valptr;
  
  switch(num)
    {
    case 1:
      op = &ins->op1;
      break;
    case 2:
      op = &ins->op2;
      break;
    case 3:
      op = &ins->op3;
      break;   
    }
  if (op->ptr && (op->content & ASM_OP_VALUE)) {
    switch (op->len) {
    case 0:
      break;
    case 1:
      if ((op->type & ASM_OP_BASE) || (op->type & ASM_OP_INDEX)) {
	if ((op->type & ASM_OP_SCALE) || (op->type & ASM_OP_INDEX)) {
	  off = 2;
	len = 1;
	} else {
	  off = 1;
	  len = 1;
	} } else {
	  off = 0;
	  len = 1;
	}
      break;
    case 2:
      if ((op->type & ASM_OP_BASE) || (op->type & ASM_OP_INDEX)) {
	off = 1;
	len = 1;
      } else {
	off = 0;
	len = 2;
      }
      break;
    case 3:
      off = 2;
      len = 1;
      break;
    case 4:
      off = 0;
      len = 4;
      break;
    case 5:
      off = 1;
      len = 4;
      break;
    case 6:
      off = 2;
      len = 4;
      break;
    } /* !switch */ } /* !if */
  else {
    off = 0;
    len = 0;
  }
  memcpy(op->ptr + off, value, len);
  memset(&op->imm, 0, 4);
  memcpy(&op->imm, value, len);
  return (len);
}


/**
 * returns a pointer to a static buffer containing instruction memonic
 */
/**
 * returns a pointer to a static buffer containing instruction memonic
 * @param ins Pointer to the instruction structure 
 * @param proc Pointer to the processor structure
 * return Return the mnemonic.
*/

char	*asm_instr_get_memonic(asm_instr *ins, asm_processor *proc) {
  return (proc->instr_table[ins->instr]);
}

/**
 * Return content field of an operand.
 * @param ins Pointer to an instruction structure.
 * @param num Number of the operand to get content from.
 * @param opt Currently not used.
 * @param valptr Currently not used.
 * @return Return operand content field.
 */

int	asm_operand_get_content(asm_instr *ins, int num, int opt, void *valptr) {
  switch(num)
    {
    case 1:
      return (ins->op1.content);
    case 2:
      return (ins->op2.content);
    case 3:
      return (ins->op3.content);
    default:
      return (0);
    }
}

/**
 * Dump debug output of operand to a file stream.
 * @param ins Pointer to the instruction structure
 * @param num Number of the operand to dump
 * @param opt optional parameter. Currently not used.
 * @param valptr is a filestream : FILE *
 * @return 1 on success, 0 on error.
 */

int	asm_operand_debug(asm_instr *ins, int num, int opt, void *valptr) {
  asm_operand	*op;
  FILE		*fp;

  switch(num)
    {
    case 1: op = &ins->op1; break;
    case 2: op = &ins->op1; break;
    case 3: op = &ins->op1; break;
    default: return (-1);
    }
  fp = (FILE *) valptr;

  fprintf(fp, "o%i len       = %i\n", num, op->len);
  fprintf(fp, "o%i ptr       = %8p\n", num, op->ptr);
  fprintf(fp, "o%i type      = %08x\n", num, op->type);
  fprintf(fp, "o%i size      = %i\n", num, op->size);
  fprintf(fp, "o%i content   = %i\n", num, op->content);

  fprintf(fp, "o%i immediate = %08X\n", num, op->imm);
  fprintf(fp, "o%i basereg   = %i\n", num, op->base_reg);
  fprintf(fp, "o%i indexreg  = %i\n", num, op->index_reg);
  fprintf(fp, "o%i scale     = %i\n", num, op->scale);
  return (1);
}

