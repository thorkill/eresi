/*
** Author  : <sk at devhell dot org>
** Started : Wed Jul 24 18:45:15 2002
** Updated : Sat Mar 20 05:26:26 2004
*/
#include <libasm.h>
#include <libasm-int.h>

/**\file generic.c */

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

int	asm_instr_len(asm_instr *i) {
  return (asm_instruction_get_len(i, 0, 0));
}



int	asm_int_pow2(int i) {
  return (i ? (2 * asm_int_pow2(i - 1)) :  1);
}

/*
 *
 */

void	asm_set_resolve_handler(asm_processor *proc, 
			        void (*fcn)(void *, u_int, char *, u_int), void *d) {
  proc->resolve_immediate = fcn;
  proc->resolve_data = d;
}

/*
** returns 1 if OPLEN is not activated
*/

int     asm_proc_is_protected(asm_processor *proc) {
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->mode == INTEL_PROT)
    return (1);
  return (0);
}


/**
 *
 *
 */

int     asm_proc_oplen(asm_processor *proc) {
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->oplen)
    return (1);
  return (0);
}

/*
 * returns 4 or 2 depending on eventual OPLEN prefix
 */

int     asm_proc_vector_len(asm_processor *proc) {
  if (asm_proc_oplen(proc))
    return (2);
  else
    return (4);
}

/**
 *
 *
 */

int	asm_proc_vector_size(asm_processor *proc) {
  if (asm_proc_oplen(proc))
    return (ASM_OSIZE_WORD);
  else
    return (ASM_OSIZE_DWORD);
}

/*
 * set the immediate part of the operand if present
 * return number of bytes written
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

char	*asm_instr_get_memonic(asm_instr *ins, asm_processor *proc) {
  return (proc->instr_table[ins->instr]);
}


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

