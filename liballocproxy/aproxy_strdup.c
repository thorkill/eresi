/*
** A version of the strdup function that proxy allocation
**
** Made by jfv for the ERESI project
**
**
** $Id$
**
*/
#include "aproxy.h"
#include <string.h>

char            *aproxy_strdup(const char *s)
{
  size_t        size;
  void          *dup;

  if (!s)
    return (NULL);
  size = strlen((char *) s);
  dup = (void *) aproxy_malloc(size + 1);
  if (!dup)
    return (NULL);
  memcpy(dup, (char *) s, size + 1);
  return ((char *) dup);
}
