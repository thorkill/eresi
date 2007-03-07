/*
** readlnheap.c for elfsh
** 
** Handle the readline malloc/free to avoid messing with the alloc proxy
**
** Started on  Sat Jul 1 10:10:53 2006 mayhem
**
** $Id: rlheap.c,v 1.2 2007-03-07 16:45:34 thor Exp $
**
*/
#include "elfsh.h"



/* When inside elfsh and not e2dbg, readline should allocate with the real malloc */
void	*vm_readline_malloc(unsigned int sz)
{
  return (malloc(sz));
}

/* When inside elfsh and not e2dbg, readline should free with the real malloc */
void	vm_readline_free(void *ptr)
{
  free(ptr);
}

