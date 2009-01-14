/**
* @file readlnheap.c
* @ingroup elfsh 
* @brief Handle the readline malloc/free to avoid messing with the alloc proxy.
*
* Started on  Sat Jul 1 10:10:53 2006 jfv
*
* $Id$
*
*/
#include "elfsh.h"



/**
 * When inside elfsh and not e2dbg, readline should allocate 
 * with the real malloc. 
 * @param sz
 * @return
 */
void	*revm_readline_malloc(unsigned int sz)
{
  return (malloc(sz));
}

/**
 * When inside elfsh and not e2dbg, readline should free with 
 * the real malloc 
 * @param ptr
*/
void	revm_readline_free(void *ptr)
{
  free(ptr);
}

