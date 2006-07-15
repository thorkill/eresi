
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: history.c,v 1.2 2006-07-15 17:06:07 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjr_history_update(mjrSession *x, asm_instr inst)
{


 if (inst.instr == ASM_NOP) {
  return 1;
 }

  x->ihist[3] = x->ihist[2];
  x->ihist[2] = x->ihist[1];
  x->ihist[1] = x->ihist[0];
  x->ihist[0] = inst;

  return 1;
}
