/*
** altgot.c for libelfsh
** 
** This file contain the architecture -dependant- code 
** of the ALTGOT technique.
**
** Started on  Wed Jun 08 21:20:07 2005 mm
**
*/
#include "libelfsh.h"



/* 
   Shift the relocation HI16 and L16 for mips binaries
   We have to do that using a signature since no relocation 
   tables are available in ET_EXEC files on mips.

   3c1c0000        lui     gp,0x0
   279c0000        addiu   gp,gp,0

   diff contains the difference to add to the offset for obtaining
   .alt.got address that we need to add to the HI16 relocations 
   matching the signature.

*/
void			elfsh_shift_mips_relocs(elfshobj_t *file, elfsh_Addr diff)
{
  uint32_t		*dword;
  uint32_t		*dword2;
  elfshsect_t	        *current;
  uint32_t		dw, dw2;
  char			patch;
  char			*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First re-relocate some entries */
  for (current = file->sectlist; current; current = current->next)
    for (dword = (uint32_t *) current->data, dword2 = dword + 1;
	 ((char *) dword + 8) <= ((char *) current->data + current->shdr->sh_size);
	 dword++, dword2++)
      {
	dw  = *dword  & 0xFFFF0000;
	dw2 = *dword2 & 0xFFFF0000;
	if (dw == 0x3c1c0000 && dw2 == 0x279c0000)
	  {

	    patch = ((!strcmp(current->name, ELFSH_SECTION_NAME_TEXT) && 
		      ((char*) dword  - (char *) current->data) > 0x100) ? 0 : 1);
	    str = (patch ? "patching" : "not patching");
	    printf("[DEBUG_SHIFTMIPS] Found dw/dw2 at address " XFMT " / " XFMT " (%s) -> %s \n", 
		   (elfsh_Addr) current->shdr->sh_addr + ((char*) dword  - (char *) current->data),
		   (elfsh_Addr) current->shdr->sh_addr + ((char*) dword2 - (char *) current->data),
		   current->name, str);
	    
	    if (patch)
	      *dword2 += ((uint32_t) diff);
	    
	  }
      }

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Shift the ALPHA relocations */
int			elfsh_shift_alpha_relocs(elfshobj_t *file, char *name, elfshsect_t *altgot, u_int off)
{
  u_int			entsz;
  elfsh_Rela		*rel;
  elfsh_Addr		addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Find the relocation entry */
  entsz = elfsh_get_pltentsz(file);
  rel = (elfsh_Rela *) elfsh_get_relent_by_name(file, name);
  if (rel == NULL)
    {
#if __DEBUG_COPYPLT__	  
      printf("[DEBUG_COPYPLT] Did not find relocation entry from symbol %s \n", name);
#endif
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,"Unable to get relocation entry", -1);
    }

#if __DEBUG_COPYPLT__	  
  else
    printf("[DEBUG_COPYPLT] Found relocation entry (" AFMT ") from symbol %s \n", 
	   elfsh_get_reloffset((elfsh_Rel *) rel), name);
#endif	  
  
  /* Compute the new value */
  addr = altgot->shdr->sh_addr + (((off - 8) / entsz) * sizeof(elfsh_Addr));
  
#if __DEBUG_COPYPLT__	  
  printf("[DEBUG_COPYPLT] Setting relocation entry to " AFMT " \n", addr);
#endif	  
  
  /* Write it */
  elfsh_set_reloffset((elfsh_Rel *) rel, addr);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Shift the SPARC relocation to make points the entries inside .alt.plt instead of .plt */
int			elfsh_shift_generic_relocs(elfshobj_t *file, elfsh_Addr diff, elfshsect_t *relplt)
{
  elfsh_Rela		*l;
  int			index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < relplt->shdr->sh_size / sizeof(elfsh_Rela); index++)
    {
      l = (elfsh_Rela *) relplt->data + index;
      elfsh_set_reloffset((elfsh_Rel *) l, elfsh_get_reloffset((elfsh_Rel *) l) + diff);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Shift relocation on IA32 for allowing non-present symbol resolving */
int			elfsh_shift_ia32_relocs(elfshobj_t *file, 
						elfsh_Addr  diff,
						elfshsect_t *relplt,
						elfsh_Addr  limit)
{
  elfsh_Rel		*l;
  int			index;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < relplt->shdr->sh_size / sizeof(elfsh_Rel); index++)
    {
      l = (elfsh_Rel *) relplt->data + index;
      if (ELFSH_NOLIMIT == limit || elfsh_get_reloffset((elfsh_Rel *) l) >= limit)
	elfsh_set_reloffset((elfsh_Rel *) l, elfsh_get_reloffset((elfsh_Rel *) l) + diff);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Same arch independant code for the SPARC case */
int			elfsh_shift_sparc_relocs(elfshobj_t	*file, 
						 elfsh_Addr	diff, 
						 elfshsect_t	*relplt)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_shift_generic_relocs(file, diff, relplt)));
}
					       



/* Change the DT_PLTGOT entry in the .dynamic section to change the relocation base address */
int			elfsh_redirect_pltgot(elfshobj_t *file, elfshsect_t *altgot, elfshsect_t *got, 
					      elfshsect_t *plt, elfshsect_t *altplt)
{
  elfsh_Sym		*sym;
  elfsh_Dyn		*dyn;
  elfshsect_t		*relplt;
  char			*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get the DT_PLTGOT entry in .dynamic */
  dyn = elfsh_get_dynamic_entry_by_type(file, DT_PLTGOT);
  if (!dyn)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DT_PLTGOT", -1);

  /* Get the PLT related relocation table */
  name = IS_REL(plt) ? ELFSH_SECTION_NAME_RELPLT : ELFSH_SECTION_NAME_RELAPLT;
  relplt = elfsh_get_section_by_name(plt->parent, name, 0, 0, 0);
  if (!relplt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find RELPLT section by name", -1);

  /* On MIPS we change it from .got to .alt.got : ALTGOT technique */
  if (FILE_IS_MIPS(file) || FILE_IS_IA32(file))
    {
      elfsh_set_dynentry_val(dyn, altgot->shdr->sh_addr);

      if (FILE_IS_MIPS(file))
	{
	  elfsh_set_gpvalue(file, altgot->shdr->sh_addr + 0x8000 - 0x10);
	  sym = elfsh_get_dynsymbol_by_name(file, "_gp_disp");
	  if (sym == NULL)
	        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Could not find _gp_disp ",  -1);
	  sym->st_value = altgot->shdr->sh_addr + 0x8000 - 0x10;
	  elfsh_shift_mips_relocs(file, altgot->shdr->sh_addr - got->shdr->sh_addr);
	}
      else
	elfsh_shift_ia32_relocs(file, 
				altgot->shdr->sh_addr - got->shdr->sh_addr, relplt, 
				ELFSH_NOLIMIT);
    }
  
  /* On SPARC we change it from .plt to .alt.plt : ALTPLT technique */
  else if (FILE_IS_SPARC(file))
    {
      elfsh_set_dynentry_val(dyn, altplt->shdr->sh_addr);
      elfsh_shift_sparc_relocs(file, altplt->shdr->sh_addr - plt->shdr->sh_addr, relplt);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


