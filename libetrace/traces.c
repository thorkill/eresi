/**
 * @file traces.c
** traces.c for libelfsh
** 
** @brief All functions that help to trace content
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: traces.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"
#include "libetrace-intvars.h"

/**
 * Create a new trace 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*elfsh_traces_createtrace(char *trace)
{
  hash_t		*newhash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  elfsh_traces_inittrace();

  /* Create a new trace tracename */
  snprintf(buf, BUFSIZ - 1, ELFSH_TRACES_PATTERN, trace);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newhash, sizeof(hash_t), NULL);
  hash_init(newhash, strdup(buf), 30, ASPECT_TYPE_UNKNOW);

  hash_add(&traces_table, strdup(trace), (void *) newhash);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newhash);
}

/* Init the table and add the default trace (global) */
int			*elfsh_traces_inittrace()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  /* Do we already set default table */
  if (traces_table.ent == NULL)
    {
      hash_init(&traces_table, ELFSH_TRACES_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);
      elfsh_traces_createtrace(ELFSH_TRACES_TYPE_DEFAULT);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Retreive a trace from its name 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*elfsh_traces_gettrace(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  elfsh_traces_inittrace();

  /* Default trace by default */
  if (!trace)
    trace = ELFSH_TRACES_TYPE_DEFAULT;

  table = hash_get(&traces_table, trace);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);
}

