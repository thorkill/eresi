/**
** @file delbreak.c
** @ingroup user
*/
#include "libe2dbg.h"


/* Vector handler for userland breakpoint deletion */
int		e2dbg_delbreak_user(elfshbp_t *bp)
{
  *(u_char *) bp->addr = bp->savedinstr[0];
  return (0);
}

