/*
** @file display.c
** 
** @brief Contain printing functions for debug purpose only
**
** FIXME: needs to use revm_output() and not printf inside elfsh.
**
** Started on  Tue Jan 02 04:04:18 2006 jfv
**
** $Id: display.c,v 1.16 2007-08-07 07:13:27 may Exp $
**
*/
#include "libmjollnir.h"

///**
// * Block dump for debug purposes 
// * @param b mjrblock strucutre
// */
//void		mjr_block_dump(mjrblock_t *b) 
//{
//  printf("[B]=(%lX) [V]=(%lX) sz=(%04u) \n", 
//	 (unsigned long) b, (unsigned long) b->vaddr, b->size);
//}


/**
 * @brief Display all information about a block 
 */
int			mjr_block_display(mjrcontext_t *ctxt, container_t *cur, mjropt_t *disopt)
{
  listent_t		*curent;
  mjrlink_t		*ccal;
  char			*str;
  char			*end_str;
  elfsh_SAddr		offset;
  elfsh_SAddr		end_offset;
  char			buf1[30];
  char			buf2[30];
  mjrblock_t		*block;
  mjrblock_t		*tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  block = cur->data;
  str = elfsh_reverse_metasym(disopt->file, block->vaddr, &offset);
  
  end_str = elfsh_reverse_metasym(disopt->file, 
				  block->vaddr + block->size, &end_offset);
  
  if (str == NULL)
    *buf1 = 0x00;
  else
    snprintf(buf1, sizeof (buf1), "<%s + " UFMT ">", str, offset);

  if (end_str == NULL || !(mjr_get_link_by_type(cur->outlinks, MJR_LINK_BLOCK_COND_TRUE)))
    *buf2 = 0x00;
  else
    snprintf(buf2, sizeof (buf2), "<%s + " UFMT ">", end_str, end_offset);
  
  printf("[%8lx:%05i] %-30s --> %-30s ", 
	 (unsigned long) block->vaddr, block->size, buf1, buf2);
  
  /*  
      if (cur->false == 0xFFFFFFFF)
      printf(" [?]");
      else if (cur->false != NULL)
      {
      str = elfsh_reverse_metasym(disopt->file, cur->false, &offset);
      printf(" [%s + " UFMT "]", (str ? str : ""), offset);
      }
  */      

  printf("\n");
  if (disopt->level > 0)
    for (curent = cur->inlinks->head; curent; curent = curent->next) 
      {
	ccal = (mjrlink_t *) curent->data;
	tmp = mjr_lookup_container(ctxt, ccal->id)->data;
	str = elfsh_reverse_metasym(disopt->file, tmp->vaddr, &offset);
	printf("\texecuted from: (" AFMT ") <%s + " UFMT ">\n",
	       tmp->vaddr, (str ? str : ""), (elfsh_SAddr) offset);
      }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ++disopt->counter);
}


/**
 * @brief Print the content of the control flow section 
 */
int			mjr_blocks_display(mjrcontext_t	*c, int level)
{
  mjropt_t		opt;
  container_t	*block;
  char			**keys;
  int			index;
  int			blocnbr;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  opt.counter = 0;
  opt.level   = level;
  opt.file    = c->obj;
  keys        = hash_get_keys(&c->blkhash, &blocnbr);
  
  for (index = 0; index < blocnbr; index++)
    {
      block = hash_get(&c->blkhash, keys[index]);
      mjr_block_display(c, block, &opt);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, opt.counter);
}

/**
 * @brief Print information for functions 
 */
void		mjr_function_display(mjrfunc_t *func)
{
  printf(" - FUNC ["AFMT"] %30s MD5: %s \n", func->vaddr, func->name, func->md5);
}

/**
 * @brief Print the functions in the function hash for the current context 
 */
void		 mjr_funcs_display(mjrcontext_t *c)
{
  char		**keys;
  int		 keynbr;
  int		 index;
  mjrfunc_t	 *current;
  container_t *cntnr;
  
  keys = hash_get_keys(&c->funchash, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      cntnr = hash_get(&c->funchash, keys[index]);
      current = cntnr->data;
      mjr_function_display(current);
    }
  hash_free_keys(keys);
}


/**
 * @brief Shortcut for vaddr 2 string with allocation .. 
 */
char	*_vaddr2str(elfsh_Addr addr)
{
  char *tmp;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tmp, BSIZE_SMALL, NULL);
  snprintf(tmp, BSIZE_SMALL, AFMT, addr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, tmp);
}

