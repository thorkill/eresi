/**
 * @defgroup libstderesi Lib Standard for eresi.
 */
/**
 * @file vlist.c
 * @ingroup libstderesi
 * @brief List all variables currently existing.
 * 
 * Updated Feb 07 2005 22:34:01 jfv
 * 
 * $Id: vlist.c,v 1.1 2007-11-29 14:01:56 may Exp $
 */
#include "libstderesi.h"

/**
 * List content of all ERESI variables and expressions
 */
int		cmd_vlist()
{
  char		logbuf[BUFSIZ];
  char		**keys;
  int		keynbr;
  int		index;
  regex_t	*tmp;
  int		printed;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  FIRSTREGX(tmp);
  snprintf(logbuf, BUFSIZ, " [*] ERESI root expressions : \n\n");
  revm_output(logbuf);
  keys = hash_get_keys(&exprs_hash, &keynbr);
  for (printed = index = 0; index < keynbr; index++)
    if (!tmp || !regexec(tmp, keys[index], 0, 0, 0))
      {
	/* Only print root expressions */
	if (strstr(keys[index], REVM_SEP))
	  continue;
	printed++;
	revm_expr_print(keys[index]);
	if (index + 1 < keynbr)
	  revm_output("\n\n");
	else
	  revm_output("\n");
      }
  hash_free_keys(keys);
  if (!keynbr)
    revm_output(" [*] No variable found \n\n");
  else
    {
      snprintf(logbuf, sizeof(logbuf), " [*] Printed %d expressions \n\n", printed);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
