/*
** 
** init_sparc.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Sun Nov 30 20:13:12 2003
** Updated : Thu Dec  4 03:01:07 2003
*/

#include <libasm.h>
#include <libasm-int.h>


int     fetch_sparc(asm_instr *ins, u_char *buf, u_int len, asm_processor *proc) {
  
  int	converted;
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_char	*ptr;
  int	i;
  ptr = (u_char*) &converted;
  for (i = 0; i < 4; i++)
    *(ptr + i) = *(buf + 3 - i);
  #if DEBUG_SPARC
  printf("[DIS_SPARC] big endian -> little endian : 0x%08x - ", converted);
  for (i = 31; i >= 0; i--)
    {
      printf("%i", MGETBIT(converted, i));
      if (!(i % 8))
	printf(" ");
    }
  puts("");
  #endif
#else
  memcpy(&converted, buf, 4);
#endif

  ins->proc = proc;
  ins->len = 4;
  if (MGETBIT(converted, 31) == 0)
    {
      if (MGETBIT(converted, 30) == 1)
	{
	  return sparc_decode_call(ins, (char*) &converted, len, proc);
	}
      else
	{
	  if (MGETBIT(converted, 24) == 1)
	    return sparc_decode_sethi(ins, (char*) &converted, len, proc);
	  else
	    {
	      if (MGETBIT(converted, 23) == 1)
		return sparc_decode_branch(ins, (char*) &converted, len, proc);
	      else
		return sparc_decode_other(ins, (char*) &converted, len, proc);
	    }
	  
	}
    }
  else
    {
      if (MGETBIT(converted, 13))
	return (sparc_decode_format3_imm(ins, (char*) &converted, len, proc));
      else
	return (sparc_decode_format3_rs(ins, (char*) &converted, len, proc));
    }
  printf("[DEBUG_SPARC] fetch_sparc:impossible execution path\n");
  return (-1);
}



void	asm_init_sparc(asm_processor *proc) {

  struct s_asm_proc_sparc	*inter;
  // int				i;
  
  proc->instr_table = /*malloc(sizeof(char *) * (ASM_SP_BAD + 1));
  for (i = 0; i < (ASM_SP_BAD + 1); i++)
  proc->instr_table[i] = */ sparc_instr_list;
  proc->resolve_immediate = asm_resolve_sparc;
  proc->resolve_data = 0;
  proc->fetch = fetch_sparc;
  proc->display_handle = asm_sparc_display_instr;
  
  proc->internals = inter = malloc(sizeof (struct s_asm_proc_sparc));
  
  inter->format0_branch_table = sparc_cond_list;
  inter->format3_op2_table = format3_op2_table;
  inter->format3_op3_table = format3_op3_table;
}





