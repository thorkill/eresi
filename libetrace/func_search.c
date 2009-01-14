/*
** @file func_search.c
**
** Libetrace header file in the ERESI project
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id$: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"

/**
 * Research the uniform debugging format function entry on a list of files 
 * @param files first element of a edfmtfile_t structure
 * @param name function name
 * @return pointer on a edfmtfunc_t (represent function debugging information)
 */
edfmtfunc_t	*trace_search_unifile(edfmtfile_t *files, char *name)
{
  edfmtfunc_t	*func;
  edfmtfile_t	*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (file = files ; file != NULL; file = file->next)
    {
      func = hash_get(&(file->hfunc), name);

      if (func != NULL)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 

      if (file->child)
	{
	  func = trace_search_unifile(file->child, name);
	  if (func != NULL)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL); 
}

/**
 * Research the uniform debugging format function entry 
 * @param file first element of a edfmtfile_t structure
 * @param name function name
 * @return pointer on a edfmtfunc_t (represent function debugging information)
 */
edfmtfunc_t    	*trace_search_uni(elfshobj_t *file, char *name)
{
  edfmtfunc_t	*func;
  edfmtinfo_t	*uni;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  uni = edfmt_get_uniinfo(file);

  if (!uni)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No debugging informations available", NULL); 

  /* Search on global scope */
  func = hash_get(&(uni->hfunc), name);
  if (func == NULL)
    {
      /* Search on local stop if needed */
      func = trace_search_unifile(uni->files, name);
      if (func == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Function not found", NULL); 
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func); 
}

