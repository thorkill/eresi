
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: history.c,v 1.1 2006-07-08 21:24:21 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjrHistoryUpdate(mjrSession *x, asm_instr inst)
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
