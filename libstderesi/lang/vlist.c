/**
 * @defgroup libstderesi The ERESI standard library.
 */
/**
 * @file libstderesi/lang/vlist.c
 * @ingroup libstderesi
 * @brief List all variables currently existing.
 * 
 * $Id$
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
  int		eindex;
  hash_t	*hash;
  u_char	header;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  FIRSTREGX(tmp);

  for (printed = 0, eindex = world.curjob->curscope; eindex >= 0; eindex--, header = 0)
    {
      hash = &world.curjob->recur[eindex].exprs;
      keys = hash_get_keys(hash, &keynbr);
      for (header = index = 0; index < keynbr; index++)
	if (!tmp || !regexec(tmp, keys[index], 0, 0, 0))
	  {
	    /* Only print root expressions */
	    if (strstr(keys[index], REVM_SEP))
	      continue;
	    printed++;
	    if (!header)
	      {
		snprintf(logbuf, BUFSIZ, "\n [*] %s expressions: \n\n", 
			 world.curjob->recur[eindex].funcname);
		revm_output(logbuf);
		header++;
	      }
	    revm_expr_print_by_name(keys[index], 0);
	    if (index + 1 < keynbr)
	      revm_output("\n\n");
	    else
	      revm_output("\n");
	  }
      hash_free_keys(keys);
    }

  if (!printed)
    revm_output(" [*] No variable found \n\n");
  else
    {
      snprintf(logbuf, sizeof(logbuf), "\n [*] Printed %d expressions \n\n", printed);
      revm_output(logbuf);
    }
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
