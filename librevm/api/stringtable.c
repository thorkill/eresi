/*
** @file stringtable.c
** @ingroup librevm_api
** @brief All functions about string table management
**
** Started on Sept 11 2005 mxatone
**
*/
#include "revm.h"

#define STRTABLE_IT 256

/* Table pointers */
char *strtable = NULL;
unsigned int strtable_current = 0;
unsigned int strtable_max = 0;

/**
 * Add a new element to the string table 
 *
 * @param string
 * @return
 */
int			revm_strtable_add(char *string)
{
  unsigned int			string_len;
  int			off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!string || !string[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid string", -1);

  string_len = strlen(string);

  if (strtable == NULL)
    {
      strtable_max = STRTABLE_IT;
      XALLOC(__FILE__, __FUNCTION__, __LINE__,strtable, strtable_max * sizeof(char), -1);
    }
  else if (strtable_max <= strtable_current + string_len)
    {
      /* Find the correct value */
      do {
	strtable_max += STRTABLE_IT;
      } while (strtable_max <= strtable_current + string_len);

      XREALLOC(__FILE__, __FUNCTION__, __LINE__,strtable, strtable, strtable_max * sizeof(char), -1);
    }

  /* Copy string */
  strcpy(strtable + strtable_current, string);
  strtable[strtable_current + string_len] = 0x00;

  /* Update positions */
  off = strtable_current;
  strtable_current += string_len + 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, off);
}
