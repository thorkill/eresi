/*
** @file trace_list.c
**
** Libetrace implementation in the ERESI project.
**
** Started on  Sun Jun 22 12:10:12 2007 jfv
** $Id: libetrace.h,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libetrace.h"


/**
 * Detail listing of a trace 
 * @param table trace hash table
 * @param name trace name
 * @see traces_list
 */
int		traces_list_detail(hash_t *table, char *name)
{
  char		**keys;
  unsigned int		index;
  int		keynbr;
  trace_t	*entrie;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ - 1, " ~> %s: %s\n", 
	   aspectworld.colorfieldstr("Trace name"),
	   aspectworld.colorstr(name));
  aspectworld.profile(buf);

  keys = hash_get_keys(table, &keynbr);

  if (keys && keynbr > 0)
    {
      aspectworld.profile("\n");

      for (index = 0; index < keynbr; index++)
	{
	  entrie = (trace_t *) hash_get(table, keys[index]);
	  
	  if (entrie)
	    {
	      snprintf(buf, BUFSIZ - 1, " %s: %s %s: %s\n",
		       aspectworld.colorfieldstr("Function name"),
		       aspectworld.colorstr_fmt("%-15s", entrie->funcname), 
		       aspectworld.colorfieldstr("status"),
		       aspectworld.colortypestr(entrie->enable ? "enabled" : "disabled"));
	      aspectworld.profile(buf);
	      aspectworld.endline();
	    }
	}

      aspectworld.profile("\n");

      hash_free_keys(keys);
    }
  else
    {
      aspectworld.profile("\n [*] No function in this trace\n\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Enumerate each trace and detail every functions 
 * @param name not use
 * @param optarg not use
 * @see traces_list_detail
 */
int		traces_list(elfshobj_t *file, char *name, char **optarg)
{
  char		**keys = NULL;
  unsigned int		index;
  int		keynbr;
  hash_t	*subtable;
  char		funcreg[256];
  size_t	len;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  aspectworld.profile(" .: Trace list :.\n");

  if (traces_table.ent)
    keys = hash_get_keys(&traces_table, &keynbr);

  if (keys)
    {
      if (keynbr > 0)
	aspectworld.profile("\n");

      for (index = 0; index < keynbr; index++)
	{
	  subtable = hash_get(&traces_table, keys[index]);

	  if (subtable)
	    traces_list_detail(subtable, keys[index]);
	}

      hash_free_keys(keys);
    }
  else
    {
      aspectworld.profile("\n [*] No tracing table available\n\n");
    }

  aspectworld.profile(" .: Exclude list :.\n");

  /* Reset */
  keynbr = 0;
  keys = NULL;

  /* Print exclude list */
  if (exclude_table.ent)
    keys = hash_get_keys(&exclude_table, &keynbr);

  if (keys)
    {
      if (exclude_table.ent)
	aspectworld.profile("\n");

      for (index = 0; index < keynbr; index++)
	{
	  len = strlen(keys[index]);
	  snprintf(funcreg, 255, "%s%s%s", 
		   keys[index][0] != '^' ? "^" : "",
		   keys[index],
		   keys[index][len-1] != '$' ? "$" : "");

	  snprintf(buf, BUFSIZ - 1, " %s %s %s %s %s\n", 
		   aspectworld.colornumber("[%02u]", index+1), 
		   aspectworld.colorfieldstr("name:"),
		   aspectworld.colorstr_fmt("%-15s", keys[index]),
		   aspectworld.colorfieldstr("regex:"),
		   aspectworld.colorstr(funcreg));
	  aspectworld.profile(buf);
	  aspectworld.endline();
	}

      hash_free_keys(keys);
    }
  else
    {
      aspectworld.profile("\n [*] No exclude table available\n");
    }

  aspectworld.profile("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

