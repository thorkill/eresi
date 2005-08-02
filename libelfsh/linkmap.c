/*
** linkmap.c for libelfsh
**
**
** Started on  Mon Jul 25 21:26:54 2005 ym
**
*/
#include "libelfsh.h"

/*
  TODO:

  - commande linkmap (affiche la linkmap)
  - loader les objets du process avant dafficher le premier prompt de debug (more param to debugger handler)
  - lobjet linkmap dans le scripting
*/



elfsh_Addr	elfsh_linkmap_get_laddr(elfshlinkmap_t *lm)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument", (NULL));
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,  lm->laddr);
}


void		elfsh_linkmap_set_laddr(elfshlinkmap_t *lm, elfsh_Addr laddr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);

  lm->laddr = laddr;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


char		*elfsh_linkmap_get_lname(elfshlinkmap_t *lm)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument",(NULL));
   
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (lm->lname));
}


void		elfsh_linkmap_set_lname(elfshlinkmap_t *lm, char *name)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
   
  
  lm->lname = name;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


elfsh_Dyn	*elfsh_linkmap_get_lld(elfshlinkmap_t *lm)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument",(NULL));
  

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,  lm->lld);
}


void		elfsh_linkmap_set_lld(elfshlinkmap_t *lm, elfsh_Dyn *lld)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);   
 
  lm->lld = lld;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


elfshlinkmap_t	*elfsh_linkmap_get_lnext(elfshlinkmap_t *lm)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument",(NULL));
   
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, lm->lnext);
}


void		elfsh_linkmap_set_lnext(elfshlinkmap_t *lm, elfshlinkmap_t *lnext)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 
  if (!lm)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);


  lm->lnext = lnext;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


elfshlinkmap_t	*elfsh_linkmap_get_lprev(elfshlinkmap_t *lm)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument",(NULL));   

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, lm->lprev);
}


void		elfsh_linkmap_set_lprev(elfshlinkmap_t *lm, elfshlinkmap_t *lprev)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!lm)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
    

  lm->lprev = lprev;

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


elfshlinkmap_t	*elfsh_linkmap_by_name(elfshlinkmap_t *lm, char *name)
{
  elfshlinkmap_t *actual;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!lm || !name)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Invalid argument",(NULL));    



  for (actual = elfsh_linkmap_get_lprev(lm); actual != NULL; actual = elfsh_linkmap_get_lprev(actual))
    if (!strcmp(name, elfsh_linkmap_get_lname(actual)))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, actual);


  for (actual = lm; actual != NULL; actual = elfsh_linkmap_get_lnext(actual))
    if (!strcmp(name, elfsh_linkmap_get_lname(actual)))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, actual);
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Linkmap entry not found", (NULL));    
}














