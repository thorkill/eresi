/*
** (C) 2001-2006 Devhell Labs / Asgard Labs : thorolf / sk / mayhem
**
** $Id: history.c,v 1.8 2007-02-18 17:01:03 may Exp $
*/
#include "libmjollnir.h"

/* Oldest entry is entry 0 */
/* It is the good way to do it */
void	mjr_history_shift(mjrcontext_t *cur, asm_instr inst, elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (inst.instr == ASM_NOP)
    ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);

  /* XXX: history should move in libasm */
  /* XXX: does not work if HISTORY_LEN is changed in the header file */
  memcpy(&cur->hist[0], &cur->hist[1], sizeof(mjrhistory_t));
  memcpy(&cur->hist[1], &cur->hist[2], sizeof(mjrhistory_t));
  memcpy(&cur->hist[2], &cur->hist[3], sizeof(mjrhistory_t));
  memcpy(&cur->hist[3], &cur->hist[4], sizeof(mjrhistory_t));
  memcpy(&cur->hist[4].instr, &inst, sizeof(asm_instr));
  cur->hist[4].vaddr = addr;

#if __DEBUG_OPERAND__
  fprintf(D_DESC,"[__DEBUG_OPERAND__] mjr_history_shift: %x/%x/%x\n",
    cur->hist[MJR_HISTORY_CUR].vaddr,
    cur->hist[MJR_HISTORY_PREV].vaddr,
    cur->hist[MJR_HISTORY_PPREV].vaddr);
#endif

  ELFSH_PROFILE_OUT(__FILE__,__FUNCTION__,__LINE__);
}

/* Write an entry of history */
void	mjr_history_write(mjrcontext_t *cur, asm_instr *i, elfsh_Addr a, int idx)
{
  if (idx >= MJR_HISTORY_LEN)
    return;

  memcpy(&cur->hist[idx].instr, i, sizeof(asm_instr));
  cur->hist[idx].vaddr = a;
}

