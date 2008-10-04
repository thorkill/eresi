/*
** @file hashes.c
** @ingroup libstderesi
** @brief Pretty printing for elfsh hash tables.
**
** Started Jan 20 2007 18:57:03 jfv
**
**
** $Id: hashes.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"

/** 
 * Display an element of a hash table
 * @param h Hash table 
 * @param key Hash key of object to be printed
 */
int		revm_table_display_element(hash_t *h, char *key, unsigned char inside)
{
  void		*data;
  char		logbuf[BUFSIZ];
  revmexpr_t	*newexpr;
  aspectype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  data = hash_get(h, key);

  if (h->type == ASPECT_TYPE_UNKNOW || !inside)
    {
      snprintf(logbuf, sizeof(logbuf), "  { %-40s = <"XFMT"> } \n", 
	       key, (eresi_Addr) data);
      revm_output(logbuf);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  if (*key == REVM_VAR_PREFIX)
    strncpy(logbuf, key, sizeof(logbuf));
  else
    snprintf (logbuf, sizeof(logbuf), "$%s", key);
  newexpr = revm_expr_get(logbuf);
  if (newexpr)
    {
      revm_output("\t");
      revm_expr_print(logbuf);
      revm_output("\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  revm_output("\t");
  if (h->type == ASPECT_TYPE_EXPR)
    {
      newexpr = (revmexpr_t *) data;
      revm_expr_print(newexpr->label);
    }
  else
    {
      type = aspect_type_get_by_id(h->type);
      newexpr = revm_inform_type_addr(type->name, strdup(logbuf), (eresi_Addr) data, NULL, 0, 1);
      if (!newexpr)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to reflect hash element to expression", -1);
      revm_expr_print(logbuf);
    }
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Display the content of a hash table  
 * @param name Hash table name to display
 */
int		revm_table_display_content(char *name)
{
  hash_t	*h;
  char		**keys;
  int		keynbr;
  int		index;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  h = hash_get(hash_hash, name);
  if (!h)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid requested hash parameter", -1);

  /* Empty hash */
  keys = hash_get_keys(h, &keynbr);
  if (!keynbr)
    {
      revm_output(" [*] Hash table is empty \n\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Display pointers */
  for (index = 0; index < keynbr; index++)
    revm_table_display_element(h, keys[index], 0);

  snprintf(logbuf, sizeof(logbuf), 
	   "\n [*] Displayed %u entries of table %s \n\n", keynbr, name);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/**
 * Display the header of a hash table 
 * @param table
 * @param name
 */
int		revm_table_display(hash_t *table, char *name)
{
  char		logbuf[BUFSIZ];
  char		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  type = revm_ascii_type(table);
  snprintf(logbuf, sizeof(logbuf), 
	   "  + %-40s\t ElemType: %-15s ElemNbr: %u \n", 
	   name, type, table->elmnbr);
  revm_output(logbuf);  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Display the content of a hash table 
 */
static void	revm_tables_display()
{
  char		**keys;
  int		keynbr;
  int		index;
  hash_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_output("  .:: Registered tables \n\n");
  keys = hash_get_keys(hash_hash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(hash_hash, keys[index]);
      revm_table_display(cur, keys[index]);
    }
  revm_output("\n Type 'help tables' for more table details.\n\n");
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Display the content of all hash tables that match the regex 
 * @param tableregx Regular expression matching table names
 * @param elemregx Regular expression matching element keys
 */
static int	revm_table_display_regx2(char *tableregx, char *elemregx)
{
  regex_t	rx, ex;
  int		keynbr, keynbr2;
  char		**keys, **keys2;
  int		index, index2;
  hash_t	*cur;
  unsigned int		match;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (regcomp(&rx, tableregx, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to compute regex", -1);
  if (regcomp(&ex, elemregx, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to compute regex", -1);

  /* Look for matching tables */
  keys = hash_get_keys(hash_hash, &keynbr);
  for (match = index = 0; index < keynbr; index++)
    if (!regexec(&rx, keys[index], 0, 0, 0))
      {
	cur = hash_get(hash_hash, keys[index]);
	keys2 = hash_get_keys(cur, &keynbr2);
	for (index2 = 0; index2 < keynbr2; index2++)
	  if (!regexec(&ex, keys2[index2], 0, 0, 0))
	    {
	      match++;
	      revm_table_display_element(cur, keys2[index2], 1);
	    }
      }
  snprintf(logbuf, sizeof(logbuf), 
	   "\n [*] Matched %u entries in all tables\n\n", match);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/** 
 * Display the content of all hash tables that match the regex 
 * @param regx 
 */
static int	revm_table_display_regx(char *regx)
{
  regex_t	rx;
  int		keynbr;
  char		**keys;
  int		index;
  int		match;
  char		buf[50];
  char		*lastmatch;
  hash_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (regcomp(&rx, regx, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to compute regex", -1);

  /* Look for matching tables */
  keys = hash_get_keys(hash_hash, &keynbr);
  for (lastmatch = NULL, match = index = 0; index < keynbr; index++)
    {
      if (!regexec(&rx, keys[index], 0, 0, 0))
	{
	  cur = hash_get(hash_hash, keys[index]);
	  revm_table_display(cur, keys[index]);
	  match++;
	  lastmatch = keys[index];
	}
    }

  /* Print the content of the table if we had a unique match */
  if (match == 1)
    revm_table_display_content(lastmatch);
  else
    {
      snprintf(buf, sizeof(buf), "\n [*] Matched %u table%c \n\n", 
	       match, (match > 1 ? 's' : ' '));
      revm_output(buf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  

}


/**
 *  Print and modify internal hash tables 
 */
int		cmd_tables()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (world.curjob->curcmd->argc)
    {
      /* Just print table list */
    case 0:
      revm_tables_display();
      break;
      
      /* Print a determined table with determined (or not) key entry */
    case 1:
      if (revm_table_display_regx(world.curjob->curcmd->param[0]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to print matching tables", -1);
      break;

      /* Print an element from a table */
    case 2:
      if (revm_table_display_regx2(world.curjob->curcmd->param[0],
				   world.curjob->curcmd->param[1]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to print matching tables's elements", -1);
      break;

      /* Unknown mode */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid tables syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Empty a hash table 
 */
int		cmd_empty()
{
  char		buf[BUFSIZ];
  hash_t	*hash;
  list_t	*list;
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (index = 0; index < world.curjob->curcmd->argc; index++)
    {
      hash = hash_find(world.curjob->curcmd->param[index]);
      if (!hash)
	{
	  list = elist_find(world.curjob->curcmd->param[index]);
	  if (!list)
	    {
	      snprintf(buf, sizeof(buf), " [W] Unknown list or hash table %s \n\n",
		       world.curjob->curcmd->param[index]);
	      revm_output(buf);
	      continue;
	    }
	  snprintf(buf, sizeof(buf), "   .:: Empty list %s \n\n",
		   world.curjob->curcmd->param[index]);
	  revm_output(buf);
	  elist_empty(world.curjob->curcmd->param[index]);
	}      
      else
	{
	  snprintf(buf, sizeof(buf), "   .:: Empty hash table %s \n\n",
		   world.curjob->curcmd->param[index]);
	  revm_output(buf);
	  hash_empty(world.curjob->curcmd->param[index]);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
