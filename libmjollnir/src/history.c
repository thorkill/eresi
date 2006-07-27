
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: history.c,v 1.3 2006-07-27 16:50:53 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjr_history_update(mjrSession *x, asm_instr inst)
{


 if (inst.instr == ASM_NOP) {
  return 1;
 }

  x->cur->ihist[3] = x->cur->ihist[2];
  x->cur->ihist[2] = x->cur->ihist[1];
  x->cur->ihist[1] = x->cur->ihist[0];
  x->cur->ihist[0] = inst;

  return 1;
}
