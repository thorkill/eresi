/*
** reginfo.c for libelfsh
**
** Started on  Wed Mar 23 14:18:20 2005 ym
** Updated on  Wed Mar 23 14:21:32 2005 mayhem
**
*/
#include "libelfsh.h"


/* Get gpvalue */
elfsh_Sword	elfsh_get_gpvalue(elfshobj_t* file)
{
  elfsh_Sword	*gp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  gp = elfsh_get_gpvalue_addr(file);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (*gp));
}


/* Set gpvalue */
int		elfsh_set_gpvalue(elfshobj_t* file, elfsh_Addr gp)
{
  elfsh_Sword	rgp;
  elfsh_Sword	*agp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  rgp = (elfsh_Sword) gp;
  agp = elfsh_get_gpvalue_addr(file);
  if (!agp)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GP value", -1);
  *agp = rgp;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Return a pointer on ".reginfo" ri_gp_value */
elfsh_Sword	*elfsh_get_gpvalue_addr(elfshobj_t* file)
{
  elfshsect_t     *reginfo;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  reginfo = elfsh_get_section_by_name(file,".reginfo", NULL, NULL, NULL);
  
  if (reginfo == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No .reginfo section", NULL);

  if (reginfo->data == NULL) 
    reginfo->data = elfsh_load_section(file, reginfo->shdr);

  if (reginfo->data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't read .reginfo section", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (&((elfsh_RegInfo *) elfsh_get_raw(reginfo))->ri_gp_value));
}


