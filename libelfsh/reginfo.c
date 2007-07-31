/**
 * @file reginfo.c
** reginfo.c for libelfsh
** 
** Started on  Wed Mar 23 14:18:20 2005 ym
** Updated on  Wed Mar 23 14:21:32 2005 jfv
** 
**
** $Id: reginfo.c,v 1.5 2007-07-31 03:28:46 may Exp $
**
*/
#include "libelfsh.h"


/**
 * Get gpvalue 
 */
elfsh_Sword	elfsh_get_gpvalue(elfshobj_t* file)
{
  elfsh_Sword	*gp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  gp = elfsh_get_gpvalue_addr(file);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*gp));
}


/**
 * Set gpvalue 
 */
int		elfsh_set_gpvalue(elfshobj_t* file, elfsh_Addr gp)
{
  elfsh_Sword	rgp;
  elfsh_Sword	*agp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  rgp = (elfsh_Sword) gp;
  agp = elfsh_get_gpvalue_addr(file);
  if (!agp)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GP value", -1);
  *agp = rgp;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return a pointer on ".reginfo" ri_gp_value 
 */
elfsh_Sword	*elfsh_get_gpvalue_addr(elfshobj_t* file)
{
  elfshsect_t     *reginfo;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  reginfo = elfsh_get_section_by_name(file,".reginfo", NULL, NULL, NULL);
  
  if (reginfo == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No .reginfo section", NULL);

  if (reginfo->data == NULL) 
    reginfo->data = elfsh_load_section(file, reginfo->shdr);

  if (reginfo->data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Can't read .reginfo section", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (&((elfsh_RegInfo *) elfsh_get_raw(reginfo))->ri_gp_value));
}

