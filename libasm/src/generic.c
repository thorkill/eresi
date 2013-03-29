/**
 * @file generic.c
 * @ingroup libasm_engine
 * Latest edition Author : $Author: may $
 * $Id$
 * Started : Wed Jul 24 18:45:15 2002
 * Updated : Sat Mar 20 05:26:26 2004
 */
#include <libasm.h>
#include <libasm-int.h>

/** 
 * Fetch instruction using asm_processor handler
 * func asm_read_instr
 * fetch instruction stored in buffer of max length len
 * according to specified processor.
 * @param instr instruction to fill.
 * @param buf pointer to opcode to disassemble
 * @param len maximum length of opcode to disassemble
 * @return -1 on error or instruction fetched length
 */

int	asm_read_instr(asm_instr *instr, u_char *buf, 
		       u_int len, asm_processor *proc) 
{
  int	to_ret;
  
  LIBASM_PROFILE_FIN();
  memset(instr, 0, sizeof (asm_instr));
  instr->proc = proc;
  to_ret = proc->fetch(instr, buf, len, proc);
  instr->name = instr->proc->instr_table[instr->instr];
  
  /* Print debug information if requested */
  if ((int) config_get_data(CONFIG_USE_ASMDEBUG))
    asm_instruction_debug(instr, stdout);
  
  LIBASM_PROFILE_FOUT(to_ret);
}

/**
 * @brief Reassemble instruction operands. WORK IN PROGRESS
 * 
 * @param instr Pointer to instruction structure.
 * @param buf Pointer to instruction buffer
 * @param len Length of buffer.
 * @return Length of written instruction or -1 on error.
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
  return (-1);
}

/**
 * @brief Return instruction length
 * @param ins Pointer to instruction structure.
 * @return 
 */

int	asm_instr_len(asm_instr *ins) 
{
  return (asm_instruction_get_len(ins, 0, 0));
}

/**
 * Return 2 power val
 * @param val The level of power to raise 2.
 * @return Return the computed value.
 */
int     asm_int_pow2(int val) {
  return (val ? (2 * asm_int_pow2(val - 1)) :  1);
}

/**
 * @brief Set the resolving handler to display addresses.
 * @param proc Pointer to processor structure.
 * @param fcn The resolving handler to use
 */

void	asm_set_resolve_handler(asm_processor *proc, 
			        void (*fcn)(void *, eresi_Addr, char *, u_int), void *d) 
{
  proc->resolve_immediate = fcn;
  proc->resolve_data = d;
}

/**
 * @brief returns 1 if OPLEN is not activated
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
 * @brief returns the value of the processor's current operand length
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
 * @brief Returns the value of the processor's current address size
 * @param proc Pointer to the processor structure
 * @return 1 if addsize is set, 0 if not 
 */

int     asm_proc_addsize(asm_processor *proc) 
{
  asm_i386_processor       *i386p;
  
  i386p = (asm_i386_processor *) proc;
  if (i386p->internals->addsize)
    return (1);
  return (0);
}

/**
 * @brief Returns vector current size depending on oplen prefix
 * This is related to ia32 architecture only
 * @param proc Pointer to the processor structure.
 * @return 4 or 2 depending on
 */

int     asm_proc_vector_len(asm_processor *proc) 
{
  if (asm_proc_opsize(proc))
    return (2);
  else
    return (4);
}

/**
 * @brief Return vector size depending on prefix
 * This is ia32 related.
 * @param proc Pointer to the processor structure
 * @return Return vector size.
 */

int	asm_proc_vector_size(asm_processor *proc) 
{
  if (asm_proc_opsize(proc))
    return (2);
  else
    return (4);
}

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

  op = 0;
  off = len = 0;
  value = (int *) valptr;
  
  switch(num)
  { 
    case 1:
      op = &ins->op[0];
      break;
    case 2:
      op = &ins->op[1];
      break;
    case 3:
      op = &ins->op[2];
      break;   
    default:
      op = 0;
      return (-1);
      break;
  }

  if (op->ptr && 
       (op->type == ASM_OPTYPE_IMM ||
          ((op->type == ASM_OPTYPE_MEM) && (op->memtype & ASM_OP_VALUE))))
  {
    switch (op->len)
    {
      case 0:
        break;
      case 1:
        if (op->type == ASM_OPTYPE_MEM &&
             ((op->memtype & ASM_OP_BASE) || (op->memtype & ASM_OP_INDEX)))
        {
        	if ((op->memtype & ASM_OP_SCALE) || (op->memtype & ASM_OP_INDEX))
          {
        	  off = 2;
          	len = 1;
        	}
          else
          {
        	  off = 1;
        	  len = 1;
        	}
        }
        else
        {
      	  off = 0;
      	  len = 1;
      	}
        break;
      case 2:
        if (op->type == ASM_OPTYPE_MEM &&
             ((op->memtype & ASM_OP_BASE) || (op->memtype & ASM_OP_INDEX)))
        {
        	off = 1;
        	len = 1;
        }
        else
        {
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
    } /* !switch */
  } /* !if */
  else
  {
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

char	*asm_instr_get_memonic(asm_instr *ins, asm_processor *proc) 
{
  return (proc->instr_table[ins->instr]);
}


/**
 * @brief Return content field of an operand.
 * @param ins Pointer to an instruction structure.
 * @param num Number of the operand to get content from.
 * @param opt Currently not used.
 * @param valptr Currently not used.
 * @return Return operand content field.
 */
int	asm_operand_get_content(asm_instr *ins, int num, int opt, void *valptr) 
{
  switch(num)
    {
    case 1:
      return (ins->op[0].content);
    case 2:
      return (ins->op[1].content);
    case 3:
      return (ins->op[2].content);
    default:
      return (0);
    }
}

/**
 * @brief Dump debug output of operand to a file stream.
 * @param ins Pointer to the instruction structure
 * @param num Number of the operand to dump
 * @param opt optional parameter. Currently not used.
 * @param valptr is a filestream : FILE *
 * @return 1 on success, 0 on error.
 */
int		asm_operand_debug(asm_instr *ins, int num, int opt, void *valptr) 
{
  asm_operand	*op;
  FILE		*fp;

  switch(num)
    {
    case 1: op = &ins->op[0]; break;
    case 2: op = &ins->op[1]; break;
    case 3: op = &ins->op[2]; break;
    default: return (-1);
    }
  if (op->content)
    {
      fp = (FILE *) valptr;

      fprintf(fp, "o%i content=[%s] type=[%s]",
	      num,
	      asm_operand_content_string(op->content),
        asm_operand_type_string(op->type));

      if (op->type == ASM_OPTYPE_MEM)
        fprintf(fp, " memtype=[%c%c%c%c]",
	        op->memtype & ASM_OP_BASE ? 'B' : ' ',
	        op->memtype & ASM_OP_INDEX ? 'I' : ' ',
	        op->memtype & ASM_OP_SCALE ? 'S' : ' ',
	        op->memtype & ASM_OP_VALUE ? 'V' : ' ');

      fprintf(fp, "\n");

      /*
      fprintf(fp, "o%i len       = %i\n", num, op->len);
      fprintf(fp, "o%i ptr       = %8p\n", num, op->ptr);
      fprintf(fp, "o%i type      = %08x\n", num, op->type);
      fprintf(fp, "o%i size      = %i\n", num, op->size);
      fprintf(fp, "o%i content   = [%i]\n", num, op->content);

      fprintf(fp, "o%i immediate = %08X\n", num, op->imm);
      fprintf(fp, "o%i basereg   = %i\n", num, op->baser);
      fprintf(fp, "o%i indexreg  = %i\n", num, op->indexr);
      fprintf(fp, "o%i scale     = %i\n", num, op->scale);
      */
    }
  return (1);
}

/**
 * Dump debugging information for an instruction.
 * @param ins Pointer to instruction to debug.
 * @param out Output FILE* stream.
 */

void	asm_instruction_debug(asm_instr *ins, FILE *out)
{
  int	i;

  fprintf(out, "ins=[%s] len=[%i] types=[%c|%c%c%c]\n",
	  asm_instr_get_memonic(ins, ins->proc), 
	  asm_instr_len(ins),
	  ins->type & ASM_TYPE_IMPBRANCH ? 'I' :
	  ins->type & ASM_TYPE_CALLPROC ? 'C' : 
	  ins->type & ASM_TYPE_CONDBRANCH ? 'J' : 
	  ins->type & ASM_TYPE_RETPROC ? 'R' : ' ',
	  ins->type & ASM_TYPE_ARITH ? 'c' : ' ',
	  ins->type & ASM_TYPE_CONTROL ? 'f' : ' ',
	  ins->type & ASM_TYPE_ASSIGN ? 'a' : ' ');
  for (i = 0; i < 3; i++)
    {
      asm_operand_debug(ins, i, 0, out);
    }
}


/**
 * set config flag to specified endian
 * @param mode endian (big/little)
 */
void asm_config_set_endian(int mode)
{
  config_update_key(CONFIG_ASM_ENDIAN_FLAG,(void *) mode);
}

/**
 * get the endian flag
 */
int asm_config_get_endian()
{
  return (int) config_get_data(CONFIG_ASM_ENDIAN_FLAG);
}

/**
 *
 *
 */

int	asm_instruction_is_prefixed(asm_instr *ins, int prefix)
{
  return (ins->prefix & prefix);
}
