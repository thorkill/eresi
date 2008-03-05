/**
** @file lists.c 
**
** @brief Pretty printing for ERESI lists
**
** Started Jul 13 2007 18:57:03 jfv
** $Id: lists.c,v 1.1 2007-11-29 14:01:56 may Exp $
*/
#include "libstderesi.h"


/** 
 * Display the content of a list 
 * @param name 
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

  /* Display pointers */
  for (cur = h->head; cur; cur = cur->next)
    {
      switch (h->type)
	{
	case ASPECT_TYPE_BYTE:
	case ASPECT_TYPE_SHORT:
	case ASPECT_TYPE_INT:
	case ASPECT_TYPE_LONG:
	case ASPECT_TYPE_DADDR:
	case ASPECT_TYPE_CADDR:
	  snprintf(logbuf, sizeof(logbuf), "  { %-20s = <"XFMT"> } \n", 
		   cur->key, (elfsh_Addr) cur->data);
	  break;
	default:
	  snprintf(logbuf, sizeof(logbuf), "  { %-20s = <"XFMT"> } (value = %hhu) \n", 
		   cur->key, (elfsh_Addr) cur->data, *(u_char *) cur->data);
	}
      revm_output(logbuf);
    }

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
 * Display the content of a hash table 
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

      /* Unknown mode */
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Invalid lists syntax", -1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
