/*
** (C) 2001-2006 Devhell Labs / Asgard Labs : thorolf / sk / mayhem
**
** $Id: history.c,v 1.5 2006-12-29 22:38:47 may Exp $
*/
#include "libmjollnir.h"

/* Oldest entry is entry 0 */
/* It is the good way to do it */
void	mjr_history_shift(mjrcontext_t *cur, asm_instr inst, elfsh_Addr addr)
{
  if (inst.instr == ASM_NOP)
    return;
  memcpy(&cur->hist[0], &cur->hist[1], sizeof(mjrhistory_t));
  memcpy(&cur->hist[1], &cur->hist[2], sizeof(mjrhistory_t));
  memcpy(&cur->hist[2], &cur->hist[3], sizeof(mjrhistory_t));
  memcpy(&cur->hist[3], &cur->hist[4], sizeof(mjrhistory_t));
  memcpy(&cur->hist[4].instr, &inst, sizeof(asm_instr));
  cur->hist[4].vaddr = addr;
  return;
}

/* Write an entry of history */
void	mjr_history_write(mjrcontext_t *cur, asm_instr *i, elfsh_Addr a, int idx)
{
  if (idx >= MJR_HISTORY_LEN)
    return;
  memcpy(&cur->hist[idx].instr, i, sizeof(asm_instr));
  cur->hist[idx].vaddr = a;
}

