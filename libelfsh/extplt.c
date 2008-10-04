/**
 * @file extplt.c
 * @ingroup libelfsh
 * extplt.c for libelfsh
 * 
 * This file contain the architecture -dependant- code 
 * for the EXTPLT technique (unknown symbol resolution)
 *
 * Started on  Wed Jun 12 21:20:07 2005 mm
 *
 * $Id: extplt.c,v 1.20 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libelfsh.h"

/**
 * Add a new entry on ALTVERSYM (.elfsh.version)
 * @param file elfsh object.
 * @param versym section pointer of target section
 * @param name function name
 */
int 		elfsh_extplt_expand_versym(elfshobj_t *file, elfshsect_t *versym, char *name)
{
  elfsh_Verneed	*verneed;
  elfsh_Verdef	*sym_verdef;
  elfsh_Vernaux	*auxneed;
  elfshobj_t	*sym_file;
  char		*filename;
  size_t	len;
  unsigned int		index;
  elfsh_Half	*ent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !versym || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid Parameters", -1);

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) != ELFSH_OS_LINUX)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Search the correct file using hash version section 
   as every linker should do */
  sym_file = elfsh_hash_getfile_def(file, name);
  if (!sym_file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't find a file for this function", -1);

  if (sym_file->id == file->id)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Same file for def", -1);  

  /* We search the filename (we have a path) */
  len = strlen(sym_file->name);
  filename = sym_file->name;

  for (index = len; index > 0; index--)
    if (sym_file->name[index] == '/')
      break;
  
  if (INTERVAL(0, index+1, len))
    filename = sym_file->name + index + 1;

  /* Retrieve symbol verdef/verneed pointer */
  verneed 	= elfsh_get_verneed_by_name(file, filename);
  sym_verdef 	= elfsh_hash_getdef(sym_file, name, NULL, 0);

  if (!verneed || !sym_verdef)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Can't find version definition / need for this symbol", -1);  

  /* If we have a good entry we just have to add a symbol, else we should create a new
     verneed entry linked with verdef references */
  auxneed = elfsh_check_defneed_eq(file, sym_file, verneed, sym_verdef);
  if (auxneed == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find equivalence between def and need", -1);  

  /* Add an entry */
  ent = versym->data + versym->curend;
  *ent = (elfsh_Half) auxneed->vna_other & 0x7fff;
  versym->curend += sizeof(elfsh_Half);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/**
 * Add a new entry on ALTHASH (.elfsh.hash)
 * @param file
 * @param hash section pointer of target section
 * @param dynsym
 * @param name function name
 */
int 		elfsh_extplt_expand_hash(elfshobj_t *file, elfshsect_t *hash, 
					 elfshsect_t *dynsym, char *name)
{
  elfsh_Word	strhash;
  elfsh_Word	nchain;
  elfsh_Word	nbucket;
  elfsh_Word	*bucket;
  elfsh_Word	*chain;
  elfsh_Word	symid;
  elfsh_Word	index;
  elfsh_Word	symindex;
  void		*data;
  unsigned char	set = 0;
  elfsh_Sym	*ret;
  int		size = 0;
  char		*actual;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !hash || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid Parameters", -1);

  /* Dynsym data & size */
  ret = elfsh_get_raw(dynsym);

  size = dynsym->curend ? dynsym->curend / sizeof(elfsh_Sym) : 
    dynsym->shdr->sh_size / sizeof(elfsh_Sym);

  if (ret == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get DYNSYM", -1);

  /* Find current symbol index */
  for (symindex = 0; symindex < size; symindex++)
    {
      actual = elfsh_get_dynsymbol_name(file, ret + symindex);
      if (actual && !strcmp(actual, name))
	break;
    }

  if (symindex >= size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find dynamic symbol by name", -1);

  /* Hash pointer */
  data = elfsh_get_raw(hash);

  /* Get bucket & chain pointers */
  bucket = elfsh_get_hashbucket(data);
  chain = elfsh_get_hashchain(data);

  /* Get parts numbers */
  nchain = elfsh_get_hashnchain(data);
  nbucket = elfsh_get_hashnbucket(data);

  strhash = elfsh_get_symbol_hash(name);

  /* Loop */
  for (index = bucket[strhash % nbucket]; index < nchain; index = symid)
    {
      symid = chain[index];

      if (symid == STN_UNDEF)
	{
	  /* Set our symbol @ tail. Next element undefine */
	  elfsh_set_hashchain_value(&chain[index], symindex);
	  chain[symindex] = STN_UNDEF;
	  set = 1;
	  break;
	}
    }

  if (set == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Chain end not found", -1);

  /* Update chain number */
  *(((elfsh_Word *) data)+1) += 1;
  hash->curend += sizeof(elfsh_Word);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/**
 * When performing EXTPLT technique, we need to mirror some sections
 * if we want to be able to extend them : .rel(a).{got,dyn,plt}, .dynsym, .dynstr 
 * @param file
 */
int		elfsh_extplt_mirror_sections(elfshobj_t *file) 
{
  elfshsect_t	*dynsym;
  elfshsect_t	*dynstr;
  elfshsect_t	*relplt;
  elfshsect_t	*versym = NULL;
  elfshsect_t	*hash;
  elfshsect_t	*dynamic;
  elfshsect_t	*enew;
  elfsh_Dyn	*dynent;
  elfsh_Dyn   	*versyment;
  elfsh_Dyn   	*hashent;
  elfshsect_t	*relgot;
  char		*data;
  char		*name;
  char		*relgotname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Grab needed pointers */
  dynsym  = file->secthash[ELFSH_SECTION_DYNSYM];
  dynstr  = file->secthash[ELFSH_SECTION_DYNSTR];
  dynamic = file->secthash[ELFSH_SECTION_DYNAMIC];
  hash    = file->secthash[ELFSH_SECTION_HASH];

  if (!dynsym || !dynstr || !dynamic || !hash)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Call ALTPLT (elfsh_copy_plt) first", -1);

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) == ELFSH_OS_LINUX)
    {
      versym = file->secthash[ELFSH_SECTION_GNUVERSYM];

      if (!versym)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Call ALTPLT (elfsh_copy_plt) first", -1);
    }
  
  /* grab relocation tables */
  name = (FILE_IS_SPARC(file) ? ELFSH_SECTION_NAME_RELAPLT : ELFSH_SECTION_NAME_RELPLT);
  relplt = elfsh_get_section_by_name(file, name, 0, 0, 0);
  if (!relplt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find PLT relocation table", -1);

  /* Pretty ugly code that needs to be cleaned up in a separate function */
  relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELGOT, 0, 0, 0);
  if (!relgot)
    {
      relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELDYN, 0, 0, 0);
      if (!relgot)
	{
	  relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELABSS, 0, 0, 0);
	  if (!relgot)
	    {
	      relgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_RELADYN, 0, 0, 0);
	      relgotname = ELFSH_SECTION_NAME_ALTRELADYN;
	    }
	  else
	    relgotname = ELFSH_SECTION_NAME_ALTRELBSS;
	}
      else
	relgotname = ELFSH_SECTION_NAME_ALTRELDYN;
    }
  else
    relgotname = ELFSH_SECTION_NAME_ALTRELGOT;

  fprintf(stderr, "found relgotname = %s \n", relgotname);


  /* Copy a double sized .dynsym somewhere else */
  enew = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTDYNSYM, NULL,
			     ELFSH_DATA_INJECTION, dynsym->shdr->sh_size * 4, 
			     sizeof(eresi_Addr));
  if (!enew)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTDYNSYM", -1);
  enew->shdr->sh_entsize = sizeof(elfsh_Sym);
  data = elfsh_get_raw(enew);

  /* Take the fixed ondisk version of dynsym as original data */
  memcpy(data, dynsym->data, dynsym->shdr->sh_size);
  
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_SYMTAB);
  if (!dynent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_SYMTAB", -1);
  elfsh_set_dynentry_val(dynent, enew->shdr->sh_addr);
  enew->curend = dynsym->shdr->sh_size;
  enew->shdr->sh_type = dynsym->shdr->sh_type; 
  file->secthash[ELFSH_SECTION_DYNSYM] = enew;
  elfsh_sync_sorted_symtab(enew);

  /* Same for dynstr */
  enew = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTDYNSTR, NULL,
			     ELFSH_DATA_INJECTION, dynstr->shdr->sh_size * 2, 
			     sizeof(eresi_Addr));
  if (!enew)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTDYNSTR", -1);
  data = elfsh_get_raw(enew);
  memcpy(data, elfsh_get_raw(dynstr), dynstr->shdr->sh_size);

  dynent = elfsh_get_dynamic_entry_by_type(file, DT_STRTAB);
  if (!dynent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_STRTAB", -1);
  elfsh_set_dynentry_val(dynent, enew->shdr->sh_addr);
  enew->curend = dynstr->shdr->sh_size;
  enew->shdr->sh_type = dynstr->shdr->sh_type;
  file->secthash[ELFSH_SECTION_DYNSTR] = enew;

  /* Cross references those last 2 sections */
  elfsh_set_section_link(file->secthash[ELFSH_SECTION_DYNSYM]->shdr, enew->index);
  elfsh_set_section_link(enew->shdr, file->secthash[ELFSH_SECTION_DYNSYM]->index);
  
  /* Same for .rel.got, if relgot is NULL then there is no .rel.got, its not an error */
  if (relgot)
    {
      enew = elfsh_insert_section(file, relgotname, NULL, ELFSH_DATA_INJECTION,
				 relgot->shdr->sh_size, sizeof(eresi_Addr));
      if (!enew)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to inject ALTRELGOT", -1);

      data = elfsh_get_raw(enew);
      memcpy(data, elfsh_get_raw(relgot), relgot->shdr->sh_size);
      dynent = elfsh_get_dynamic_entry_by_type(file, IS_REL(dynamic) ? DT_REL : DT_RELA);
      if (!dynent)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get DT_REL", -1);
      elfsh_set_dynentry_val(dynent, enew->shdr->sh_addr);
      enew->curend = relgot->shdr->sh_size;
      enew->shdr->sh_type = relgot->shdr->sh_type;
      enew->shdr->sh_link = file->secthash[ELFSH_SECTION_DYNSYM]->index;
      enew->shdr->sh_entsize = IS_REL(dynamic) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
    }

  /* Same for .rel.plt */
  enew = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTRELPLT, NULL,
			     ELFSH_DATA_INJECTION, relplt->shdr->sh_size * 4, 
			     sizeof(eresi_Addr));
  if (!enew)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to inject ALTRELPLT", -1);
  data = elfsh_get_raw(enew);
  memcpy(data, elfsh_get_raw(relplt), relplt->shdr->sh_size);
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_JMPREL);
  if (!dynent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_RELPLT", -1);
  elfsh_set_dynentry_val(dynent, enew->shdr->sh_addr);
  enew->curend = relplt->shdr->sh_size;
  enew->shdr->sh_type = relplt->shdr->sh_type;
  enew->shdr->sh_link = file->secthash[ELFSH_SECTION_DYNSYM]->index;
  enew->shdr->sh_entsize = IS_REL(dynamic) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) == ELFSH_OS_LINUX)
    {
      /* Same for .gnu.version */
      enew = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTVERSYM, NULL,
				 ELFSH_DATA_INJECTION, versym->shdr->sh_size * 4, 
				 sizeof(eresi_Addr));
      if (!enew)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to inject ALTVERSYM", -1);
      data = elfsh_get_raw(enew);
      memcpy(data, elfsh_get_raw(versym), versym->shdr->sh_size);

      enew->curend = versym->shdr->sh_size;
      enew->shdr->sh_type = versym->shdr->sh_type;
      enew->shdr->sh_link = file->secthash[ELFSH_SECTION_GNUVERSYM]->index;
      enew->shdr->sh_entsize = sizeof(elfsh_Half);
      file->secthash[ELFSH_SECTION_VERSYM] = enew;

      /* Redirect on .dynamic section */
      versyment = elfsh_get_dynamic_entry_by_type(file, DT_VERSYM);
      if (!versyment)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to get DT_VERSYM", -1);
      elfsh_set_dynentry_val(versyment, enew->shdr->sh_addr);
    }

  /* Same for .hash */
  enew = elfsh_insert_section(file, ELFSH_SECTION_NAME_ALTHASH, NULL,
			     ELFSH_DATA_INJECTION, hash->shdr->sh_size * 4, 
			     sizeof(eresi_Addr));
  if (!enew)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to inject ALTHASHM", -1);
  data = elfsh_get_raw(enew);
  memcpy(data, elfsh_get_raw(hash), hash->shdr->sh_size);
  
  enew->curend = hash->shdr->sh_size;
  enew->shdr->sh_type = hash->shdr->sh_type;
  enew->shdr->sh_link = file->secthash[ELFSH_SECTION_HASH]->index;
  enew->shdr->sh_entsize = sizeof(elfsh_Word);
  file->secthash[ELFSH_SECTION_HASH] = enew;

  /* Redirect on .dynamic section */
  hashent = elfsh_get_dynamic_entry_by_type(file, DT_HASH);
  if (!hashent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get DT_HASH", -1);
  elfsh_set_dynentry_val(hashent, enew->shdr->sh_addr);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Insert a new function entry point for dynamic resolution
 * Return the symbol pointing on this new definition (its .plt entry) 
 *
 * @param file
 * @param name
 * @return
 */
elfsh_Sym	*elfsh_request_pltent(elfshobj_t *file, char *name)	
{
  unsigned int		sz;
  elfshsect_t	*relplt;
  elfshsect_t	*extplt;
  elfshsect_t	*altgot;
  elfshsect_t	*dynsym;
  elfshsect_t	*dynstr;
  elfshsect_t	*altversym = NULL;
  elfshsect_t	*althash;
  unsigned int		relentsz;
  elfsh_Sym	sym;
  unsigned int		len;
  elfsh_Dyn	*dynent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  len = strlen(name);

  // This information is not always interering, add it when you need it
  // add a DEBUG_ define, but do not commit uncommented.
  //fprintf(stderr, "Requesting a new PLT entry for symbol %s \n", name);

  /* Get needed sections */
  extplt = file->secthash[ELFSH_SECTION_EXTPLT];
  if (!extplt)
    {
      extplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_EXTPLT,
					 NULL, NULL, NULL);
      if (extplt == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "EXTPLT not found : Copy PLT first", NULL);
      file->secthash[ELFSH_SECTION_EXTPLT] = extplt;
    }

  relplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTRELPLT, 
				     0, 0, 0);
  if (!relplt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find RELPLT section", NULL);

  altgot = file->secthash[ELFSH_SECTION_ALTGOT];
  if (!altgot)
    {
      altgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTGOT,
					 NULL, NULL, NULL);
      if (!altgot)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "ALTGOT not found : Copy PLT first", NULL);
      file->secthash[ELFSH_SECTION_ALTGOT] = altgot;
    }

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) == ELFSH_OS_LINUX)
    {
      altversym = file->secthash[ELFSH_SECTION_VERSYM];
      if (!altversym)
	{
	  altversym = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_VERSYM,
						NULL, NULL, NULL);
	  file->secthash[ELFSH_SECTION_VERSYM] = altversym;
	}
    }

  althash = file->secthash[ELFSH_SECTION_HASH];
  if (!althash)
    {
      althash = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HASH,
					 NULL, NULL, NULL);
      if (!althash)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "ALTHASH not found : Copy HASH first", NULL);
      file->secthash[ELFSH_SECTION_HASH] = althash;
    }
  
  dynsym = file->secthash[ELFSH_SECTION_DYNSYM];
  if (!dynsym)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DYNSYM section", NULL);
  dynstr = file->secthash[ELFSH_SECTION_DYNSTR];
  if (!dynstr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find DYNSTR section", NULL);

  /* Check room inside sections */
  sz = elfsh_get_pltentsz(file);
  if (extplt->curend + sz > extplt->shdr->sh_size)
    {
      if (extplt->shdr->sh_offset + extplt->curend + sz <
	  extplt->next->shdr->sh_offset)
	extplt->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in EXTPLT", NULL);
    }

  relentsz = IS_REL(extplt) ? sizeof(elfsh_Rel) : sizeof(elfsh_Rela);
  if (relplt->curend + relentsz > relplt->shdr->sh_size)
    {
      if (relplt->shdr->sh_offset + relplt->curend + sz <
	  relplt->next->shdr->sh_offset)
	relplt->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in RELPLT copy", NULL);
    }

  if (dynsym->curend + sizeof(elfsh_Sym) > dynsym->shdr->sh_size)
    {
      if (dynsym->shdr->sh_offset + dynsym->curend + sz <
	  dynsym->next->shdr->sh_offset)
	dynsym->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in DYNSYM copy", NULL);
    }
  if (dynstr->curend + len > dynstr->shdr->sh_size)
    {
      if (dynstr->shdr->sh_offset + dynstr->curend + sz <
	  dynstr->next->shdr->sh_offset)
	dynstr->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in DYNSTR copy", NULL);
    }
  if (altgot->curend + sizeof(eresi_Addr) > altgot->shdr->sh_size)
    {
      if (altgot->shdr->sh_offset + altgot->curend + sz <
	  altgot->next->shdr->sh_offset)
	altgot->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in ALTGOT", NULL);
    }

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) == ELFSH_OS_LINUX && altversym)
    {
      if (altversym->curend + sizeof(elfsh_Half) > altversym->shdr->sh_size)
	{
	  if (altversym->shdr->sh_offset + altversym->curend + sz <
	      altversym->next->shdr->sh_offset)
	    altversym->shdr->sh_size += sz;
	  else
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "No room anymore in ALTVERSYM", NULL);
	}
    }
  if (althash->curend + sizeof(elfsh_Word) > althash->shdr->sh_size)
    {
      if (althash->shdr->sh_offset + althash->curend + sz <
	  althash->next->shdr->sh_offset)
	althash->shdr->sh_size += sz;
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "No room anymore in ALTHASH", NULL);
    }
  
  /* The EXTPLT hook will allocate a new relentry and gotentry */
  if (elfsh_extplt(extplt, altgot, dynsym, relplt) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to satisfy EXTPLT handler", NULL);

  /* Change RELPLT size in .dynamic */
  dynent = elfsh_get_dynamic_entry_by_type(file, DT_PLTRELSZ);
  if (!dynent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to get DT_PLTRELSZ", NULL);
  elfsh_set_dynentry_val(dynent, elfsh_get_dynentry_val(dynent) + relentsz);

  /* Insert symbol referenced by previous injected reloc entry.
     We do not indicate a link with EXTPLT section because an external symbol
     should not be link on a local section or it will be resolv as a local symbol. */
  sym = elfsh_create_symbol(extplt->shdr->sh_addr + extplt->curend - elfsh_get_pltentsz(file),
			    elfsh_get_pltentsz(file), STT_FUNC,
			    0, 0, 0);
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
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get DT_STRSZ", NULL);
  elfsh_set_dynentry_val(dynent, elfsh_get_dynentry_val(dynent) + len + 1);

  /* Versym expand is only for linux */
  if (elfsh_get_ostype(file) == ELFSH_OS_LINUX && altversym)
    {
      /* Insert version symbol */
      if (elfsh_extplt_expand_versym(file, altversym, name) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to add an entry on ALTVERSYM", NULL);
    }

  /* Expend .hash */
  if (elfsh_extplt_expand_hash(file, althash, dynsym, name) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to add an entry on ALTHASH", NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     elfsh_get_raw(dynsym) + dynsym->curend - sizeof(elfsh_Sym));
}

