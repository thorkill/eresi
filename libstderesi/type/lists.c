/**
** @file lists.c 
** @ingroup libstderesi
** @brief Pretty printing for ERESI lists
**
** Started Jul 13 2007 18:57:03 jfv
** $Id: lists.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"


/** 
 * Display an element of a keyed linked list
 * @param h Linked list
 * @param key Key of list object to be printed
 */
int		revm_list_display_element(list_t *l, char *key, unsigned char inside)
{
  void		*data;
  char		logbuf[BUFSIZ];
  revmexpr_t	*newexpr;
  aspectype_t	*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  data = elist_get(l, key);
  if (l->type == ASPECT_TYPE_UNKNOW || !inside)
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
  if (l->type == ASPECT_TYPE_EXPR)
    {
      newexpr = (revmexpr_t *) data;
      revm_expr_print(newexpr->label);
    }
  else
    {
      type = aspect_type_get_by_id(l->type);
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
 * Display the content of a list 
 * @param name List name to display
 */
int		revm_list_display_content(char *name)
{
  list_t	*h;
  listent_t	*cur;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  h = hash_get(hash_lists, name);
  if (!h)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid requested list parameter", -1); 
  if (!h->elmnbr)
    {
      revm_output(" [*] List is empty \n\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Display list entries */
  for (cur = h->head; cur; cur = cur->next)
    revm_list_display_element(h, cur->key, 0);

  snprintf(logbuf, sizeof(logbuf), 
	   "\n [*] Displayed %u entries of list %s \n\n", h->elmnbr, name);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Display the header of a list */
int		revm_list_display(list_t *list, char *name)
{
  char		logbuf[BUFSIZ];
  char		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!list)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  type = revm_ascii_ltype(list);
  snprintf(logbuf, sizeof(logbuf), 
	   "  + %-40s\t ElemType: %-15s ElemNbr: %u \n", 
	   name, type, list->elmnbr);
  revm_output(logbuf);  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Display the content of a list 
 */
static void	revm_lists_display()
{
  char		**keys;
  int		keynbr;
  int		index;
  list_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_output("  .:: Registered lists \n\n");
  keys = hash_get_keys(hash_lists, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cur = hash_get(hash_lists, keys[index]);
      revm_list_display(cur, keys[index]);
    }
  revm_output("\n Type 'help lists' for more table details.\n\n");
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Display the content of all hash tables that match the regex 
 * @param tableregx Regular expression matching table names
 * @param elemregx Regular expression matching element keys
 */
static int	revm_list_display_regx2(char *tableregx, char *elemregx)
{
  regex_t	rx, ex;
  int		keynbr;
  char		**keys;
  int		index;
  list_t	*cur;
  listent_t	*curent;
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
  keys = hash_get_keys(hash_lists, &keynbr);
  for (match = index = 0; index < keynbr; index++)
    if (!regexec(&rx, keys[index], 0, 0, 0))
      {
	cur = hash_get(hash_hash, keys[index]);
	for (curent = cur->head; curent; curent = curent->next)
	  if (!regexec(&ex, curent->key, 0, 0, 0))
	    {
	      match++;
	      revm_list_display_element(cur, curent->key, 1);
	    }
      }
  snprintf(logbuf, sizeof(logbuf), 
	   "\n [*] Matched %u entries in all lists \n\n", match);
  revm_output(logbuf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Display the content of all hash tables that match the regex 
 */
static int	revm_list_display_regx(char *regx)
{
  regex_t	rx;
  int		keynbr;
  char		**keys;
  int		index;
  int		match;
  char		buf[50];
  char		*lastmatch;
  list_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (regcomp(&rx, regx, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to compute regex", -1);

  /* Look for matching tables */
  keys = hash_get_keys(hash_lists, &keynbr);
  for (lastmatch = NULL, match = index = 0; index < keynbr; index++)
    {
      if (!regexec(&rx, keys[index], 0, 0, 0))
	{
	  cur = hash_get(hash_lists, keys[index]);
	  revm_list_display(cur, keys[index]);
	  match++;
	  lastmatch = keys[index];
	}
    }

  /* Print the content of the table if we had a unique match */
  if (match == 1)
    revm_list_display_content(lastmatch);
  else
    {
      snprintf(buf, sizeof(buf), "\n [*] Matched %u list%c \n\n", 
	       match, (match > 1 ? 's' : ' '));
      revm_output(buf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  

}


/** 
 * Print and modify internal lists 
 */
int		cmd_lists()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (world.curjob->curcmd->argc)
    {
      /* Just print the list of lists */
    case 0:
      revm_lists_display();
      break;
      
      /* Print a determined table with determined (or not) key entry */
    case 1:
      if (revm_list_display_regx(world.curjob->curcmd->param[0]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to print matching lists", -1);
      break;

    case 2:
      if (revm_list_display_regx2(world.curjob->curcmd->param[0],
				 world.curjob->curcmd->param[1]) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to print matching list elements", -1);
      break;

      /* Unknown mode */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid lists syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
