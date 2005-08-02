/*
** ia64.c for libelfsh
**
** Started on  Sat Feb 26 23:25:37 2005 mayhem
**
** Skeleton cut & pasted from the sparc64 backend
**
*/
#include "libelfsh.h"


/* Static hooking for IA64 */
int	elfsh_cflow_ia64(elfshobj_t     *null,
			 char		*sname,
			 elfsh_Sym	*null2,
			 elfsh_Addr	null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}



// 


/* PLT hijacking on IA64 */
int		elfsh_hijack_plt_ia64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* ALTPLT hijacking on IA64 */
int		elfsh_hijack_altplt_ia64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* Perform relocation on entry for IA64 architecture */
int       elfsh_relocate_ia64(elfshsect_t       *new,
			      elfsh_Rela        *cur,
			      elfsh_Addr        *dword,
			      elfsh_Addr        addr,
			      elfshsect_t	*mod)
{
  int	  retval;
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  retval = 0;
  switch (elfsh_get_reltype((elfsh_Rel *)cur))
    {
      /* cases ... */

    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Unsupported relocation type",
		     -1);
      break;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
}


