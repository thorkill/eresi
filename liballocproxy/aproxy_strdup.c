/*
** A version of the strdup function that proxy allocation
**
** Made by mayhem for the ERESI project
**
*/
#include "aproxy.h"
#include <string.h>

char            *strdup(const char *s)
{
  size_t        size;
  void          *dup;

  if (!s)
    return (NULL);
  size = strlen((char *) s);
  dup = (void *) aproxy(size + 1);
  if (!dup)
    return (NULL);
  memcpy(dup, (char *) s, size + 1);
  return ((char *) dup);
}
