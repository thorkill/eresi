/**
 * @file altgot.c
 * @ingroup libelfsh
** altgot.c for libelfsh
** 
** This file contain the architecture -dependant- code 
** of the ALTGOT technique.
**
** Started on  Wed Jun 08 21:20:07 2005 mm
** $Id: altgot.c,v 1.7 2008-02-16 12:32:27 thor Exp $
*/
#include "libelfsh.h"



/**
 * @brief Shift the relocation HI16 and L16 for mips binaries
 * We have to do that using a signature since no relocation 
 * tables are available in ET_EXEC files on MIPS.
 *
 * 3c1c0000        lui     gp,0x0
 * 279c0000        addiu   gp,gp,0
 *
 * diff contains the difference to add to the offset for obtaining
 * .alt.got address that we need to add to the HI16 relocations 
 * matching the signature.
 *
 * @param file Parent file.
 * @param diff Absolute difference to shift with.
 * @return Nothing.
 */
void			elfsh_shift_mips_relocs(elfshobj_t *file, eresi_Addr diff)
{
  uint32_t		*dword;
  uint32_t		*dword2;
  elfshsect_t	        *current;
  uint32_t		dw, dw2;
  char			patch;
  char			*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
		   (eresi_Addr) current->shdr->sh_addr + ((char*) dword  - (char *) current->data),
		   (eresi_Addr) current->shdr->sh_addr + ((char*) dword2 - (char *) current->data),
		   current->name, str);
	    
	    if (patch)
	      *dword2 += ((uint32_t) diff);
	    
	  }
      }

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * @brief Shift the ALPHA relocation table.
 * @param file The file where to relocate the table.
 * @param name Name of symbol for which to relocate entry.
 * @param altgot Descriptor for the ALTGOT section.
 * @param off
 * @return Success (0) or Error (-1).
 */
int			elfsh_shift_alpha_relocs(elfshobj_t	*file, 
						 char		*name, 
						 elfshsect_t	*altgot, 
						 unsigned int		off)
{
  unsigned int			entsz;
  elfsh_Rela		*rel;
  eresi_Addr		addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Find the relocation entry */
  entsz = elfsh_get_pltentsz(file);
  rel = (elfsh_Rela *) elfsh_get_relent_by_name(file, name);
  if (rel == NULL)
    {
#if __DEBUG_COPYPLT__	  
      printf("[DEBUG_COPYPLT] Did not find relocation entry from symbol %s \n", name);
#endif
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,"Unable to get relocation entry", -1);
    }

#if __DEBUG_COPYPLT__	  
  else
    printf("[DEBUG_COPYPLT] Found relocation entry (" AFMT ") from symbol %s \n", 
	   elfsh_get_reloffset((elfsh_Rel *) rel), name);
#endif	  
  
  /* Compute the new value */
  addr = altgot->shdr->sh_addr + (((off - 8) / entsz) * sizeof(eresi_Addr));
  
#if __DEBUG_COPYPLT__	  
  printf("[DEBUG_COPYPLT] Setting relocation entry to " AFMT " \n", addr);
#endif	  
  
  /* Write it */
  elfsh_set_reloffset((elfsh_Rel *) rel, addr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * @brief Shift the SPARC relocation to make points the entries inside .alt.plt instead of .plt 
 * @param file The host file where shift must happens.
 * @param diff Absolute difference to shift to.
 * @param relplt Section descriptor for the .relplt section.
 * @return Always 0.
 */
int			elfsh_shift_generic_relocs(elfshobj_t *file, 
						   eresi_Addr diff, 
						   elfshsect_t *relplt)
{
  elfsh_Rela		*l;
  int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < relplt->shdr->sh_size / sizeof(elfsh_Rela); index++)
    {
      l = (elfsh_Rela *) relplt->data + index;
      elfsh_set_reloffset((elfsh_Rel *) l, elfsh_get_reloffset((elfsh_Rel *) l) + diff);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Shift relocation tables at some point for allowing non-present symbol resolving 
 * mostly applied on section injection for ET_DYN objects.
 * @param file The host file where shift must happens.
 * @param diff Absolute difference to shift to.
 * @param relplt Section descriptor for the .relplt section.
 * @param limit Upper limit address beyond which shifting must not be performed (or ELFSH_NOLIMIT)
 * @return Always 0.
 */
int			elfsh_shift_ia32_relocs(elfshobj_t *file, 
						eresi_Addr  diff,
						elfshsect_t *relplt,
						eresi_Addr  limit)
{
  elfsh_Rel		*l;
  int			index;
  eresi_Addr		reloff;
  elfshsect_t		*parent;
  elfsh_SAddr		off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (index = 0; index < relplt->shdr->sh_size / sizeof(elfsh_Rel); index++)
    {
      l = (elfsh_Rel *) relplt->data + index;
      if (ELFSH_NOLIMIT == limit || elfsh_get_reloffset((elfsh_Rel *) l) >= limit)
	{
	  reloff = elfsh_get_reloffset((elfsh_Rel *) l);
	  reloff += diff;
	  elfsh_set_reloffset((elfsh_Rel *) l, reloff);

	  /* Shifting memory -pointed- by the relative relocation */
	  if (elfsh_get_reltype(l) != R_386_RELATIVE)
	    continue;
	  parent = elfsh_get_parent_section(file, reloff, &off);
	  if (strstr(parent->name, "got") || strstr(parent->name, "bss") ||
	      strstr(parent->name, "elfsh"))
	    continue;
	  *(eresi_Addr *) ((char *) parent->data + off) += diff;
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Shift relocations on the SPARC architecture.
 * @param file Host file.
 * @param diff Difference for shift.
 * @param relplt Descriptor for the .relplt section.
 * @return Always 0.
 */
int			elfsh_shift_sparc_relocs(elfshobj_t	*file, 
						 eresi_Addr	diff, 
						 elfshsect_t	*relplt)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (elfsh_shift_generic_relocs(file, diff, relplt)));
}
					       



/**
 * @brief Change the DT_PLTGOT entry in the .dynamic section to change 
 * the relocation base address.
 * @param file The host file.
 * @param altgot Section descriptor for the .elfsh.altgot section.
 * @param got Section descriptor for the .got section.
 * @param plt Section descriptor for the .plt section.
 * @param altplt Section descriptor for the .elfsh.altplt section.
 * @return Success (0) or Error (-1).
 */
int			elfsh_redirect_pltgot(elfshobj_t *file, 
					      elfshsect_t *altgot, 
					      elfshsect_t *got, 
					      elfshsect_t *plt, 
					      elfshsect_t *altplt)
{
  elfsh_Sym		*sym;
  elfsh_Dyn		*dyn;
  elfshsect_t		*relplt;
  char			*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get the DT_PLTGOT entry in .dynamic */
  dyn = elfsh_get_dynamic_entry_by_type(file, DT_PLTGOT);
  if (!dyn)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DT_PLTGOT", -1);

  /* Get the PLT related relocation table */
  name = IS_REL(plt) ? ELFSH_SECTION_NAME_RELPLT : ELFSH_SECTION_NAME_RELAPLT;
  relplt = elfsh_get_section_by_name(plt->parent, name, 0, 0, 0);
  if (!relplt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
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
	        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


