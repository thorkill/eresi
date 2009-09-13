/**
* @file libelfsh/linkmap.c
 * @ingroup libelfsh
** linkmap.c for libelfsh
** 
** TODO: Add linkmap object in scripting
** 
** Started on  Mon Jul 25 21:26:54 2005 ym
** 
**
** $Id$
**
*/
#include "libelfsh.h"

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 */

eresi_Addr	elfsh_linkmap_get_laddr(elfshlinkmap_t *lm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument", (NULL));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  lm->laddr);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @param laddr
 * @return
 */
void		elfsh_linkmap_set_laddr(elfshlinkmap_t *lm, eresi_Addr laddr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  lm->laddr = laddr;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 */
char		*elfsh_linkmap_get_lname(elfshlinkmap_t *lm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument",(NULL));
   
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (lm->lname));
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @param name
 * @return
 */
void		elfsh_linkmap_set_lname(elfshlinkmap_t *lm, char *name)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
   
  lm->lname = name;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 */
elfsh_Dyn	*elfsh_linkmap_get_lld(elfshlinkmap_t *lm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument",(NULL));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,  lm->lld);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @param lld
 * @return
 */
void		elfsh_linkmap_set_lld(elfshlinkmap_t *lm, elfsh_Dyn *lld)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);   
 
  lm->lld = lld;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 */
elfshlinkmap_t	*elfsh_linkmap_get_lnext(elfshlinkmap_t *lm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument",(NULL));
   
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, lm->lnext);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @param lnext
 * @return
 */
void		elfsh_linkmap_set_lnext(elfshlinkmap_t *lm, elfshlinkmap_t *lnext)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
  if (!lm)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  lm->lnext = lnext;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 */
elfshlinkmap_t	*elfsh_linkmap_get_lprev(elfshlinkmap_t *lm)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument",(NULL));   

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, lm->lprev);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @return
 *
 */
void		elfsh_linkmap_set_lprev(elfshlinkmap_t *lm, 
					elfshlinkmap_t *lprev)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
    

  lm->lprev = lprev;

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Documentation missing.
 * @param lm Pointer to linkmap structure.
 * @param name
 * @return
 */
elfshlinkmap_t	*elfsh_linkmap_by_name(elfshlinkmap_t *lm, char *name)
{
  elfshlinkmap_t *actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!lm || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid argument",(NULL));   

  for (actual = elfsh_linkmap_get_lprev(lm); actual != NULL; 
       actual = elfsh_linkmap_get_lprev(actual))
    if (!strcmp(name, elfsh_linkmap_get_lname(actual)))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, actual);


  for (actual = lm; actual != NULL; actual = elfsh_linkmap_get_lnext(actual))
    if (!strcmp(name, elfsh_linkmap_get_lname(actual)))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, actual);
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Linkmap entry not found", (NULL));    
}














