/**
* @file libasm/src/arch/ia32/init_i386.c
 * @ingroup ia32
 * $Id$
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

int	fetch_i386(asm_instr *instr, u_char *buf, u_int len, 
		   asm_processor *proc)
{
  u_char	opcode;
  int		(*fetch)(asm_instr *, u_char *, u_int, asm_processor *);
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
 * Intialize the processor and vector structures to disassemble i386 bytecode.
 * @param Pointer to a asm_processor structure.
 * @return Always 1.
 */

int asm_init_ia32(asm_processor *proc)
{
  struct s_asm_proc_i386 *inter;
  
  init_instr_table(proc);
  proc->type = ASM_PROC_IA32;
  asm_register_ia32();
  proc->resolve_immediate = asm_resolve_ia32;
  proc->resolve_data = 0;
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

/* Resolve destinations of control flow instruction depending on real or protected mode */
eresi_Addr	asm_dest_resolve(asm_processor *proc, eresi_Addr addr, u_int shift)
{
  int		mode;
  u_short	off;

  mode = asm_ia32_get_mode(proc);
  if (mode == INTEL_REAL)
    {
      off = (addr & 0xFFFF) + shift;
      addr = (addr & 0xFFFF0000) + off;
    }
  else
    addr += shift;
  return (addr);
}


/** Get mode of IA32 processor */
int asm_ia32_get_mode(asm_processor *proc)
{
  struct s_asm_proc_i386 *inter;

  inter = proc->internals;
  return (inter->mode);
}


/** 
 * Switch 
 * @param proc Pointer to asm processor structure.
 * @param mode  INTEL_PROT or INTEL_REAL
 */
int asm_ia32_switch_mode(asm_processor *proc, int mode)
{
  struct s_asm_proc_i386 *inter;

  inter = proc->internals;
  inter->mode = mode;
  if (mode == INTEL_PROT)
    inter->opsize = inter->addsize = 0;
  else
    {
      /**
       * inter->opsize and inter->addsize values are perhaps not accurate.
       * Must add testcode for this feature.
       */
      inter->opsize = inter->addsize = 1;
    }
  return (1);
}

#endif
