/**
 * @file func_exclude.c
** @ingroup libetrace
** 
** @brief trace table exclude functions
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_exclude.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"
//#include "libetrace-extern.h"

/**
 * Exclude functions by regex during the last stage
 * @param regstr reg string
 */
int			etrace_funcexclude(char *regstr)
{
  char			funcreg[256];
  size_t		len;
  regex_t		preg, *set;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!regstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Init table if needed */
  if (exclude_table.ent == NULL)
    hash_init(&exclude_table, ELFSH_TRACES_EXCLUDE_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);

  len = strlen(regstr);
  snprintf(funcreg, 255, "%s%s%s", 
	   regstr[0] != '^' ? "^" : "",
	   regstr,
	   regstr[len-1] != '$' ? "$" : "");

  /* Check if its a valid regexec */
  if (regcomp(&preg, funcreg, 0) != 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid regex", -1);

  /* Alloc and copy our regex */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, set, sizeof(regex_t), -1);
  memcpy(set, &preg, sizeof(regex_t));

  /* Add it to the hash table */
  hash_add(&exclude_table, strdup(regstr), set);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Remove an exclude function by regex during the last stage
 * @param regstr reg string
 */
int			etrace_funcrmexclude(char *regstr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!regstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (exclude_table.ent)
    hash_del(&exclude_table, regstr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
