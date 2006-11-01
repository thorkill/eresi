/*
** extplt.c for libelfsh
** 
** This file contain the architecture -dependant- code 
** for the EXTPLT technique (unknown symbol resolution)
**
** Started on  Wed Jun 12 21:20:07 2005 mm
**
*/
#include "libelfsh.h"




/* When performing EXTPLT technique, we need to mirror some sections
   if we want to be able to extend them : .rel.plt, .dynsym, .dynstr */
int		elfsh_extplt_mirror_sections(elfshobj_t *file) 
{
  elfshsect_t	*dynsym;
  elfshsect_t	*dynstr;
  elfshsect_t	*relplt;
  elfshsect_t	*dynamic;
  elfshsect_t	*new;
  elfsh_Dyn	*dynent;

  elfshsect_t	*relgot;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Grab needed pointers */
  dynsym  = file->secthash[ELFSH_SECTION_DYNSYM];
  dynstr  = file->secthash[ELFSH_SECTION_DYNSTR];
  dynamic = file->secthash[ELFSH_SECTION_DYNAMIC];
  if (!dynsym || !dynstr || !dynamic)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Call ALTPLT (elfsh_copy_plt) first", -1);
  
  /* grab relocation tables */
  relplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELPLT, 0, 0, 0);
  if (!relplt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find PLT relocation table", -1);
  relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELGOT, 0, 0, 0);
  if (!relgot)
    relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELDYN, 0, 0, 0);

  /* Copy a double sized .dynsym somewhere else */
  new = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTDYNSYM, NULL,
			     ELFSH_DATA_INJECTION, dynsym->shdr->sh_size * 2, 
			     sizeof(elfsh_Addr));
  if (!new)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTDYNSYM", -1);
  new->shdr->sh_entsize = sizeof(elfsh_Sym);

  memcpy(elfsh_get_raw(new), elfsh_get_raw(dynsym), dynsym->shdr->sh_size);
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_SYMTAB);
  if (!dynent)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_SYMTAB", -1);
  elfsh_set_dynentry_val(dynent, new->shdr->sh_addr);
  new->curend = dynsym->shdr->sh_size;
  new->shdr->sh_type = dynsym->shdr->sh_type; 
  file->secthash[ELFSH_SECTION_DYNSYM] = new;
  elfsh_sync_sorted_symtab(new);

  /* Same for dynstr */
  new = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTDYNSTR, NULL,
			     ELFSH_DATA_INJECTION, dynstr->shdr->sh_size * 2, 
			     sizeof(elfsh_Addr));
  if (!new)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTDYNSTR", -1);
  memcpy(elfsh_get_raw(new), elfsh_get_raw(dynstr), dynstr->shdr->sh_size);

  dynent = elfsh_get_dynamic_entry_by_type(file, DT_STRTAB);
  if (!dynent)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_STRTAB", -1);
  elfsh_set_dynentry_val(dynent, new->shdr->sh_addr);
  new->curend = dynstr->shdr->sh_size;
  new->shdr->sh_type = dynstr->shdr->sh_type;
  file->secthash[ELFSH_SECTION_DYNSTR] = new;

  /* Cross references those last 2 sections */
  elfsh_set_section_link(file->secthash[ELFSH_SECTION_DYNSYM]->shdr, new->index);
  elfsh_set_section_link(new->shdr, file->secthash[ELFSH_SECTION_DYNSYM]->index);
  
  /* Same for .rel.got, if relgot is NULL then there is no .rel.got, its not an error */
  if (relgot)
    {
      new = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTRELGOT, NULL,
				 ELFSH_DATA_INJECTION, relgot->shdr->sh_size,
				 sizeof(elfsh_Addr));
      if (!new)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to inject ALTRELGOT", -1);
      memcpy(elfsh_get_raw(new), elfsh_get_raw(relgot), relgot->shdr->sh_size);
      dynent = elfsh_get_dynamic_entry_by_type(file, DT_REL);
      if (!dynent)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get DT_REL", -1);
      elfsh_set_dynentry_val(dynent, new->shdr->sh_addr);
      new->curend = relgot->shdr->sh_size;
      new->shdr->sh_type = relgot->shdr->sh_type;
      new->shdr->sh_link = file->secthash[ELFSH_SECTION_DYNSYM]->index;
      new->shdr->sh_entsize = IS_REL(dynamic) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
    }

  /* Same for .rel.plt */
  new = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTRELPLT, NULL,
			     ELFSH_DATA_INJECTION, relplt->shdr->sh_size * 4, 
			     sizeof(elfsh_Addr));
  if (!new)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTRELPLT", -1);
  memcpy(elfsh_get_raw(new), elfsh_get_raw(relplt), relplt->shdr->sh_size);
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_JMPREL);
  if (!dynent)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_RELPLT", -1);
  elfsh_set_dynentry_val(dynent, new->shdr->sh_addr);
  new->curend = relplt->shdr->sh_size;
  new->shdr->sh_type = relplt->shdr->sh_type;
  new->shdr->sh_link = file->secthash[ELFSH_SECTION_DYNSYM]->index;
  new->shdr->sh_entsize = IS_REL(dynamic) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Insert a new function entry point for dynamic resolution */
/* Return the symbol pointing on this new definition (its .plt entry) */
elfsh_Sym	*elfsh_request_pltent(elfshobj_t *file, char *name)	
{
  u_int		sz;
  elfshsect_t	*relplt;
  elfshsect_t	*extplt;
  elfshsect_t	*altgot;
  elfshsect_t	*dynsym;
  elfshsect_t	*dynstr;
  u_int		relentsz;
  elfsh_Sym	sym;
  u_int		len;
  elfsh_Dyn	*dynent;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !name)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  len = strlen(name);

  /* Get needed sections */
  extplt = file->secthash[ELFSH_SECTION_EXTPLT];
  if (!extplt)
    {
      extplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_EXTPLT,
					 NULL, NULL, NULL);
      if (extplt == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "EXTPLT not found : Copy PLT first", NULL);
      file->secthash[ELFSH_SECTION_EXTPLT] = extplt;
    }

  relplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTRELPLT, 
				     0, 0, 0);
  if (!relplt)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find RELPLT section", NULL);

  altgot = file->secthash[ELFSH_SECTION_ALTGOT];
  if (!altgot)
    {
      altgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTGOT,
					 NULL, NULL, NULL);
      if (!altgot)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "ALTGOT not found : Copy PLT first", NULL);
      file->secthash[ELFSH_SECTION_ALTGOT] = altgot;
    }

  dynsym = file->secthash[ELFSH_SECTION_DYNSYM];
  if (!dynsym)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DYNSYM section", NULL);
  dynstr = file->secthash[ELFSH_SECTION_DYNSTR];
  if (!dynstr)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DYNSTR section", NULL);

  /* Check room inside sections */
  sz = elfsh_get_pltentsz(file);
  if (extplt->curend + sz > extplt->shdr->sh_size)
    {
      if (extplt->shdr->sh_offset + extplt->curend + sz <
	  extplt->next->shdr->sh_offset)
	extplt->shdr->sh_size += sz;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in EXTPLT", NULL);
    }

  relentsz = IS_REL(extplt) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
  if (relplt->curend + relentsz > relplt->shdr->sh_size)
    {
      if (relplt->shdr->sh_offset + relplt->curend + sz <
	  relplt->next->shdr->sh_offset)
	relplt->shdr->sh_size += sz;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in RELPLT copy", NULL);
    }

  if (dynsym->curend + sizeof(elfsh_Sym) > dynsym->shdr->sh_size)
    {
      if (dynsym->shdr->sh_offset + dynsym->curend + sz <
	  dynsym->next->shdr->sh_offset)
	dynsym->shdr->sh_size += sz;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in DYNSYM copy", NULL);
    }
  if (dynstr->curend + len > dynstr->shdr->sh_size)
    {
      if (dynstr->shdr->sh_offset + dynstr->curend + sz <
	  dynstr->next->shdr->sh_offset)
	dynstr->shdr->sh_size += sz;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in DYNSTR copy", NULL);
    }
  if (altgot->curend + sizeof(elfsh_Addr) > altgot->shdr->sh_size)
    {
      if (altgot->shdr->sh_offset + altgot->curend + sz <
	  altgot->next->shdr->sh_offset)
	altgot->shdr->sh_size += sz;
      else
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in ALTGOT", NULL);
    }

  /* The EXTPLT hook will allocate a new relentry and gotentry */
  if (elfsh_extplt(extplt, altgot, dynsym, relplt) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to satisfy EXTPLT handler", NULL);

  /* Change RELPLT size in .dynamic */
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_PLTRELSZ);
  if (!dynent)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get DT_PLTRELSZ", NULL);
  elfsh_set_dynentry_val(dynent, elfsh_get_dynentry_val(dynent) + relentsz);

  /* Insert symbol referenced by previous injected reloc entry */
  sym = elfsh_create_symbol(extplt->shdr->sh_addr + extplt->curend - elfsh_get_pltentsz(file),
			    elfsh_get_pltentsz(file), STT_SECTION,
			    0, 0, extplt->index);
  sym.st_name = dynstr->curend;
  elfsh_set_symbol_bind(&sym, STB_GLOBAL);
  memcpy(elfsh_get_raw(dynsym) + dynsym->curend, &sym, sizeof(sym));
  dynsym->curend += sizeof(elfsh_Sym);

  /* Insert string referenced by the previous injected symbol */
  memcpy(elfsh_get_raw(dynstr) + dynstr->curend, name, len + 1);
  dynstr->curend += len + 1;
  elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_DYNSYM]);

  /* Change DYNSTR size in .dynamic */
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_STRSZ);
  if (!dynent)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_STRSZ", NULL);
  elfsh_set_dynentry_val(dynent, elfsh_get_dynentry_val(dynent) + len + 1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     elfsh_get_raw(dynsym) + dynsym->curend - sizeof(elfsh_Sym));
}

