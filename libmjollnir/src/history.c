/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: history.c,v 1.4 2006-12-11 13:57:39 may Exp $
 *
 */
#include "libmjollnir.h"


int	mjr_history_update(mjrsession_t *x, asm_instr inst)
{
  if (inst.instr == ASM_NOP)
    return 1;
  x->cur->ihist[3] = x->cur->ihist[2];
  x->cur->ihist[2] = x->cur->ihist[1];
  x->cur->ihist[1] = x->cur->ihist[0];
  x->cur->ihist[0] = inst;
  return 1;
}
