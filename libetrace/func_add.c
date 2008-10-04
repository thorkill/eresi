/**
 * @file func_add.c
** @ingroup libetrace
** 
** @brief add function to a trace table.
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: func_add.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Add the function on the trace table 
 * @param trace trace name
 * @param name function name
 * @param newtrace allocated structure that describe the trace
 * @return newtrace pointer or NULL in case of failure
 */
trace_t 		*etrace_func_add(char *trace, 
					char *name,
					trace_t *newtrace)
{
  hash_t		*table;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !newtrace)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  table = etrace_get(trace);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Trace table not found", NULL);

  if (hash_get(table, name) != NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Function already exist", NULL);

  trace_enabled_count++;
  hash_add(table, newtrace->funcname, (void *) newtrace);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newtrace);
}



/**
 * Add a function on a trace (from commande line)
 * @param file Target
 * @param name function name
 * @param optarg trace name (optional)
 */
int			traces_add(elfshobj_t *file, char *name, char **optarg)
{
  trace_t		*traces;
  edfmtfunc_t		*func = NULL;
  eresi_Addr		vaddr = 0;
  unsigned char		external = 0;
  char			**func_list;
  unsigned int			index, traced = 0;
  char			*trace_name;
  int			ret;
  char			*current_name;
  eresi_Addr		addr = 0;
  unsigned char		cont = 0;
  unsigned char		multiaddr = 0;
  size_t		prelen;
  elfshsect_t		*newsymsect;
  elfsh_Sym		*symbol;
  char			buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!name || !name[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Try to match functions with a regex */
  if (trace_match_funcname(file, name, &func_list) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't match a single function", -1);

  /* Address case 
     if the function name (always the first) will have the address in its name
     else we found a symbol which correspond to the address */
  if (IS_VADDR(name) && strstr(func_list[0], name + 2) != NULL)
    sscanf(name + 2, AFMT, &addr);

  trace_name = optarg ? *optarg : NULL;

  /* When we try to match everything */
  multiaddr = TRACE_MATCH_ALL(name);
  prelen = strlen(TRACE_PRE_FUNC);

  if (multiaddr)
    {
      snprintf(buf, BUFSIZ - 1,
	       "\n\t[*] Start hooking all internal/external functions ...\n");
      aspectworld.profile(buf);
    }

  /* Try to add every function */
  for (index = 0; func_list[index] != NULL; index++)
    {
      external = 0;
      vaddr = 0;
      cont = 0;
      current_name = func_list[index];

      /* FIXME: A function name containing "." will not be compiled right with gcc */
      if (strstr(current_name, ".") || !PRINTABLE(current_name[0]))
	continue;

      /* Can we trace this function */
      if (addr > 0)
	ret = etrace_valid_faddr(file, addr, &vaddr, &external);
      else
	{
	  /* Multiple address tracing */
	  if (multiaddr && !strncmp(TRACE_PRE_FUNC, current_name, prelen))
	    {
	      /* Prepare for a new address function */
	      sscanf(current_name + prelen, AFMT, &vaddr);
	      ret = 0;
	    }
	  else
	    ret = etrace_tracable(file, current_name, &vaddr, &external);
	}

      if (ret < 0)
	{
	  buf[0] = 0x00;
	  switch(ret)
	    {
	    case -2: /* Untracable function */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s is an untracable function on this OS (context specific)\n",
		       current_name);
	      cont = 1;
	      break;

	    case -3: /* Not called address */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s (%s) address will be traced but is never called.\n",
		       current_name, name);
	      break;

	    case -4: /* Not called address */
	      snprintf(buf, BUFSIZ - 1, 
		       "\t[!] %s (%s) address can't be found on entry point section.\n",
		       current_name, name);
	      cont = 1;
	      break;
	    }

	  if (buf[0] != 0x00)
	    aspectworld.profile(buf);

	  if (cont)
	    continue;
	}

      /* Can we trace with libedfmt informations ? */
      if (addr > 0 || vaddr > 0)
	{
	  func = NULL;

	  /* Do we need to add the symbol right before hijacking ? */
	  symbol = elfsh_get_symbol_by_name(file, current_name);
	  
	  if (!symbol)
	    {
	      newsymsect = elfsh_get_parent_section(file, addr > 0 ? addr : vaddr, NULL);
	  
	      if (!newsymsect)
		continue;
	  
	      /* Add a new function symbol */
	      if (elfsh_insert_funcsym(file, current_name, 
				       addr > 0 ? addr : vaddr,
				       0, newsymsect->index) < 0)
		continue;
	    }
	  else if (!symbol->st_value) /* Invalid symbol */
	    continue;
	}
      
      if (!addr)
	func = trace_func_debug_get(file, current_name, external);

      /* Create the structure */
      traces = trace_param_create(file, current_name, func, vaddr, external);

      if (!traces)
	continue;

      /* Add the function */
      if (etrace_func_add(trace_name, current_name, traces) == NULL)
	continue;

      /* To avoid flood we don't display every added function for multiaddr */
      if (!multiaddr)
	{
	  snprintf(buf, BUFSIZ - 1, "\t[*] Added function %s successfully into trace %s\n",
		   current_name, trace_name ? trace_name : ETRACE_TYPE_DEFAULT);
	  aspectworld.profile(buf);
	}

      traced++;
    }

  /* Free allocated table */
  if (index > 0)
    XFREE(__FILE__, __FUNCTION__, __LINE__, func_list);

  /* Do we add at least something ? */
  if (traced == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't add any founded functions", -1);

  /* We print the sum only */
  if (multiaddr)
    {
      snprintf(buf, BUFSIZ - 1, "\t[*] Added %d functions successfully into trace %s\n\n",
	      traced, trace_name ? trace_name : ETRACE_TYPE_DEFAULT);
      aspectworld.profile(buf);      
    }

  /* Free function list */
  XFREE(__FILE__, __FUNCTION__, __LINE__, func_list);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
