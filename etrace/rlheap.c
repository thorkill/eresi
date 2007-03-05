/*
** readlnheap.c for etrace
** 
** Handle the readline malloc/free to avoid messing with the alloc proxy
**
** Started on  Sat Jul 1 10:10:53 2006 mxatone
*/
#include "etrace.h"



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

