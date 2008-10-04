/**
 * @file init_i386.c
 * @ingroup libasm_ia32
 * $Id: init_i386.c,v 1.13 2007/10/14 00:01:41 heroine Exp $
 *
 */
#ifndef I386_H_
#define I386_h_

#include <libasm.h>
#include <libasm-int.h>

void	init_instr_table(asm_processor *);


/**
 * Handler to fetch i386 bytecode.
 * This handler is called throught a function pointer stored in 
 * the asm_processor structure.
 *
 * @param instr Pointer to instruction structure to fill.
 * @param buf Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction or -1 on error.
 */

int	fetch_i386(asm_instr *instr, unsigned char *buf, unsigned int len, 
			      asm_processor *proc)
{
  unsigned char	opcode;
  int		(*fetch)(asm_instr *, unsigned char *, unsigned int, asm_processor *);
  int		to_ret;
  
  opcode = *buf;
  fetch = asm_opcode_fetch(LIBASM_VECTOR_OPCODE_IA32, opcode);
  to_ret = fetch(instr, buf, len, proc);
  if (to_ret > 0)
    instr->name = proc->instr_table[instr->instr];
  return (to_ret);
  
}

int	asm_init_i386(asm_processor *proc)
{
  fprintf(stderr, "you should not use asm_init_i386 anymore."
	  " Use asm_init_ia32 instead\n");
  return (asm_init_ia32(proc));
}

/**
 * Intialize the processor structure to disassemble i386 bytecode.
 * @param Pointer to a asm_processor structure.
 * @return Always 1.
 */

int asm_init_ia32(asm_processor *proc)
{
  
  struct s_asm_proc_i386 *inter;
  
  init_instr_table(proc);
  proc->type = ASM_PROC_IA32;
  /* Register the ia32 vector */
  asm_register_ia32();

  // XXX: dead code to remove
  //asm_init_ia32_vector();
  //asm_init_vectors(proc);
  // asm_arch_register(proc, 1);  /* dummy parameters for the  moment */

  
  proc->resolve_immediate = asm_resolve_ia32;
  proc->resolve_data = 0;
  // proc->get_operand = create_i386_operand;
  proc->fetch = fetch_i386;
  proc->display_handle = asm_ia32_display_instr_att;
  inter = proc->internals = malloc(sizeof (struct s_asm_proc_i386));
  inter->opsize = inter->addsize = 0;
  inter->mode = INTEL_PROT;

  return (1);
}

void asm_free_i386(asm_processor *proc) 
{
  free(proc->internals);
  free(proc->instr_table);
}

#endif
