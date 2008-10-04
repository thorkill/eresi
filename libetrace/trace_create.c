/**
 * @defgroup libetrace Libetrace Reference
 */
/**
 * @file trace_create.c
 * @ingroup libetrace
** 
** @brief Functions for managing traces creation.
** 
** Started Jul 2 2005 00:03:44 mxatone
** $Id: traces.c,v 1.2 2007-11-29 10:25:02 rival Exp $
*/
#include "libetrace.h"
#include "libetrace-intvars.h"

/* @brief Hash table of traces */
hash_t		traces_table;

/** @brief Exclude hash table which contain regex */
hash_t		exclude_table;


/**
 * @brief Create a new trace 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*etrace_create_trace(char *trace)
{
  hash_t		*newhash;
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!trace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  etrace_init_trace();

  /* Create a new trace tracename */
  snprintf(buf, BUFSIZ - 1, ETRACE_PATTERN, trace);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newhash, sizeof(hash_t), NULL);
  hash_init(newhash, strdup(buf), 30, ASPECT_TYPE_UNKNOW);
  hash_add(&traces_table, strdup(trace), (void *) newhash);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newhash);
}

/**
 * @brief Init the table and add the default trace (global) .
 */
int			*etrace_init_trace()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  /* Do we already set default table */
  if (traces_table.ent == NULL)
    {
      hash_init(&traces_table, ETRACE_TABLE_NAME, 30, ASPECT_TYPE_UNKNOW);
      etrace_create_trace(ETRACE_TYPE_DEFAULT);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Retreive a trace from its name 
 * @param trace trace name
 * @return trace hash table
 */
hash_t			*etrace_get(char *trace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  etrace_init_trace();

  /* Default trace by default */
  if (!trace)
    trace = ETRACE_TYPE_DEFAULT;

  table = hash_get(&traces_table, trace);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, table);
}



/**
 * @brief Search the function entry on the uniform debugging format, and return it 
 * @param file First element of a edfmtfile_t structure.
 * @param func_name Function name.
 * @param external Indicate if we need internal (0) or external (1) lookup.
 * @return pointer A edfmtfunc_t* representing the function debugging information.
 */
edfmtfunc_t 		*trace_func_debug_get(elfshobj_t *file, char *func_name,
							 unsigned char external)
{
  elfshobj_t		*sym_file;
  edfmtfunc_t		*func = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We have an extern symbol */
  if (external)
    {
      sym_file = elfsh_symbol_search(file, func_name);
      
      if (sym_file)
	func = trace_search_uni(sym_file, func_name);
    }
  else
    {
      /* Search in this file */
      func = trace_search_uni(file, func_name);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, func);
}


/**
 * @brief Construct the argument structure depending of the option we have 
 * @param file target file
 * @param ent pointer to set
 * @param func uniform debugging inforation if we got one
 * @param vaddr virtual address of the function if we don't have debugging information
 * @param external describe scope
 * @see trace_param_create
 */
int			trace_param_build(elfshobj_t *file, trace_t *ent, 
					  edfmtfunc_t *func, eresi_Addr vaddr, 
					  unsigned char external)
{
  unsigned int			index;
  int			*argcount;
  edfmtfuncarg_t	*arg;
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (!ent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  elfsh_setup_hooks();

  ent->scope = external ? ELFSH_ARG_EXTERN : ELFSH_ARG_INTERN;

  /* Do we have uniform debugging information for this structure ? */
  if (func == NULL)
    {
      argcount = elfsh_args_count(file, 0, vaddr);
      
      ent->type = ELFSH_ARG_SIZE_BASED;
      ent->argc = 0;

      /* If we found some argument, else its 0 */
      if (argcount != 0 && argcount != (int *) 0xffffffff)
	{
	  /* Fill arguments */
	  for (index = 0; argcount[index] > 0; index++)
	    ent->arguments[index].size = argcount[index];
	  
	  ent->argc = index;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, argcount);
	}
      else
	{
	  snprintf(buf, BUFSIZ, "\t[*] Unable to add func at "XFMT" into trace\n", vaddr);
	  aspectworld.profile(buf);
	}
    }
  else
    {
      ent->type = ELFSH_ARG_TYPE_BASED;
      for (arg = func->arguments, index = 0; arg != NULL; arg = arg->next, index++)
	{
	  /* We setup argument name and typename */
	  ent->arguments[index].name = arg->name;
	  ent->arguments[index].typename = arg->type ? arg->type->name : NULL;

	  /* Other information are quite the same as arg counting method */
	  ent->arguments[index].size = 0;
	  if (arg->type->size > 0)
	    ent->arguments[index].size = arg->type->size; 
	  
	  if (ent->arguments[index].size < sizeof(eresi_Addr))
	    ent->arguments[index].size = sizeof(eresi_Addr); 
	}

      ent->argc = func->argc;
    }
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Allocate the traces  structure and call previous
 * @param file target file
 * @param func uniform debugging inforation if we got one
 * @param vaddr virtual address of the function if we don't have debugging information
 * @param external describe scope
 * @return generate trace structure
 * @see trace_param_build
 */
trace_t		*trace_param_create(elfshobj_t *file, char *name,
				    edfmtfunc_t *func, eresi_Addr vaddr,
				    unsigned char external)
{
  trace_t 	*newtrace;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // Alloc the structure
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newtrace, sizeof(trace_t), NULL);
  snprintf(newtrace->funcname, ETRACE_FUNC_SIZE, "%s", name);
  newtrace->file = file;
  newtrace->enable = 1;
  newtrace->vaddr = vaddr;

#if __DEBUG_ARG_COUNT__
  printf("[DEBUG_ARG_COUNT] Builds args for %s\n", name);
#endif

  // We fill arguments part and check if we found the function
  if (trace_param_build(file, newtrace, func, vaddr, external) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, newtrace);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Symbol not found or impossible to trace", NULL);
     } 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}



/**
 * Create a trace 
 * @param file target
 * @param name trace name
 * @param optarg list of function to directly add
 */
int		traces_create(elfshobj_t *file, char *name, char **optarg)
{
  char		*strarr[2];
  int		index;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (etrace_create_trace(name) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Create trace failed", -1);

  snprintf(buf, BUFSIZ - 1, "\t[*] Created trace %s successfully\n\n", name);
  aspectworld.profile(buf);

  if (optarg && *optarg)
    {
      /* Add gived elements */
      for (index = 0; optarg[index]; index++)
	{
	  strarr[0] = name;
	  strarr[1] = NULL;
	  if (traces_add(file, optarg[index], strarr) < 0)
	    continue;
	}

      aspectworld.profile("\n");
    }
    
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
