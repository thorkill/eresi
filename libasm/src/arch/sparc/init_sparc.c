/*
** 
** init_sparc.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Sun Nov 30 20:13:12 2003
** Updated : Thu Dec  4 03:01:07 2003
**
** $Id: init_sparc.c,v 1.10 2007/11/28 00:21:37 strauss Exp $
**
*/

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetching handler of the sparc architecture.
 * @param ins
 * @param buf
 * @param len
 * @param proc
 */ 
int fetch_sparc(asm_instr *ins, unsigned char *buf, unsigned int len, asm_processor *proc) 
{ 
  vector_t *vec;
  unsigned int dim[3];
  int (*fetch)(asm_instr *, unsigned char *, unsigned int, asm_processor *); 
  
  int converted;  
  
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned char *ptr;
  int i;
  ptr = (unsigned char*) &converted;
  
  for (i = 0; i < 4; i++)
    *(ptr + i) = *(buf + 3 - i);
    
  #if DEBUG_SPARC
    printf("[DIS_SPARC] big endian -> little endian : 0x%08x - ", converted);
    
    for (i = 31; i >= 0; i--){
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
  ins->ptr_instr = buf;
  ins->nb_op = 0;
  ins->type = ASM_TYPE_NONE;
  /* Primary (implicit) address space */
  ins->op[0].address_space = 0x80;
  ins->op[1].address_space = 0x80;
  ins->op[2].address_space = 0x80;
  
  vec = aspect_vector_get(LIBASM_VECTOR_OPCODE_SPARC);
  dim[0] = (converted & 0xC0000000) >> 30;
  dim[1] = 0;
  dim[2] = 0;
  
  if (MGETBIT(converted, 31)) {
  	if (MGETBIT(converted, 30)) {  	  	  
  	  dim[1] = (converted >> 19) & 0x3f;
  	  dim[2] = 0;
  	}
    else {
      dim[1] = (converted >> 19) & 0x3f;

      if (dim[1] == 0x35) /* FPop2 */
  	    dim[2] = (converted & 0x3E0) >> 5;
  	  else
  	    dim[2] = 0;
    }
  }	
  else {
  	if (MGETBIT(converted, 30)) {
  	  dim[1] = 0;
  	  dim[2] = 0;
  	}
    else {
      dim[1] = (converted >> 22) & 0x7;
  	  dim[2] = 0;	  
    }
  }
  
  fetch = aspect_vectors_select(vec, dim);
  return (fetch(ins, (unsigned char*) &converted, len, proc));
  
  printf("[DEBUG_SPARC] fetch_sparc:impossible execution path\n");
  return (-1);
}

/**
 * Initialize the sparc processor.
 *
 */

int	asm_init_sparc(asm_processor *proc) 
{
  struct s_asm_proc_sparc	*inter;
  
  proc->instr_table = sparc_instr_list;
  proc->resolve_immediate = asm_resolve_sparc;
  proc->resolve_data = 0;
  proc->fetch = fetch_sparc;
  proc->display_handle = asm_sparc_display_instr;
  proc->type = ASM_PROC_SPARC;
  
  proc->internals = inter = malloc(sizeof (struct s_asm_proc_sparc));
  
  inter->bcc_table = sparc_bcc_list;
  inter->brcc_table = sparc_brcc_list;
  inter->fbcc_table = sparc_fbcc_list;
  inter->shift_table = sparc_shift_list;
  inter->movcc_table = sparc_movcc_list;
  inter->movfcc_table = sparc_movfcc_list;
  inter->movr_table = sparc_movr_list;
  inter->fpop1_table = sparc_fpop1_list;
  inter->fmovcc_table = sparc_fmovcc_list;
  inter->fmovfcc_table = sparc_fmovfcc_list;
  inter->fmovr_table = sparc_fmovr_list;
  inter->fcmp_table = sparc_fcmp_list;
  inter->tcc_table = sparc_tcc_list; 
  inter->op2_table = sparc_op2_table;
  inter->op3_table = sparc_op3_table;
 

  /**
   * XXX: Check this code and update if necessary to follow line developpement.
   */
  
  asm_arch_register(proc, 0);
  return (1);
}
