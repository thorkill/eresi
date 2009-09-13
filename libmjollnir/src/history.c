/**
* @file libmjollnir/src/history.c
 * @ingroup libmjollnir
 *
 * (C) 2001-2008 Devhell Labs / Asgard Labs : thorolf / sk / jfv
 * 
 */

#include "libmjollnir.h"

/* Oldest entry is entry 0 */
/* It is the good way to do it */
void	mjr_history_shift(mjrcontext_t *cur, asm_instr inst, eresi_Addr addr)
{
  int i;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if ((cur->proc.type == ASM_PROC_IA32 && inst.instr == ASM_NOP) ||
      (cur->proc.type == ASM_PROC_SPARC && inst.instr == ASM_SP_NOP))
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
  
  for (i = 0; i < MJR_HISTORY_LEN - 1; i++) 
    memcpy(&cur->hist[i], &cur->hist[i + 1], sizeof(mjrhistory_t));
  mjr_history_write(cur, &inst, addr, MJR_HISTORY_CUR);
  
#if __DEBUG_OPERAND__
  fprintf(D_DESC,"[__DEBUG_OPERAND__] mjr_history_shift: %x/%x/%x\n",
	  cur->hist[MJR_HISTORY_CUR].vaddr,
	  cur->hist[MJR_HISTORY_PREV].vaddr,
	  cur->hist[MJR_HISTORY_PPREV].vaddr);
#endif
  
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/* Write an entry of history */
void	mjr_history_write(mjrcontext_t *cur, asm_instr *i, eresi_Addr a, int idx)
{
  if (idx >= MJR_HISTORY_LEN)
    return;
  
  memcpy(&cur->hist[idx].instr, i, sizeof(asm_instr));
  cur->hist[idx].vaddr = a;
}

