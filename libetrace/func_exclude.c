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
#include "libetrace.h"


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
    hash_init(&exclude_table, ETRACE_EXCLUDE_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);

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


/**
 * Exclude functions by regexec during the last stage
 * @param file target
 * @param freg first reg
 * @param oreg others reg list
 */
int		traces_exclude(elfshobj_t *file, char *freg, char **oreg)
{
  unsigned int		index;
  char		buf[BUFSIZ];
  const char	pattern[] = "\n\t[*] Exclude function %s successfully\n\n";

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!freg || !freg[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Exclude first argument */
  if (etrace_funcexclude(freg) < 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  snprintf(buf, BUFSIZ - 1, pattern, freg);
  aspectworld.profile(buf);

  if (oreg)
    {
      for (index = 0; oreg[index] != NULL; index++)
	{
	  if (etrace_funcexclude(oreg[index]) < 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

	    snprintf(buf, BUFSIZ - 1, pattern, oreg[index]);
	    aspectworld.profile(buf);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Delete excluded functions by regexec during the last stage
 * @param file target
 * @param freg first reg
 * @param oreg others reg list
 */
int		traces_rmexclude(elfshobj_t *file, char *freg, char **oreg)
{
  unsigned int		index;
  const char	pattern[] = "\n\t[*] Delete excluded function %s successfully\n\n";
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!freg || !freg[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  etrace_funcrmexclude(freg);

  snprintf(buf, BUFSIZ - 1, pattern, freg);
  aspectworld.profile(buf);

  if (oreg)
    {
      for (index = 0; oreg[index] != NULL; index++)
	{
	  etrace_funcrmexclude(oreg[index]);

	  snprintf(buf, BUFSIZ - 1, pattern, oreg[index]);
	  aspectworld.profile(buf);
	}
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
