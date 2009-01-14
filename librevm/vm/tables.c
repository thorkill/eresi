/**
** @defgroup librevm_vm librevm VM
** @file tables.c
** @ingroup librevm_vm
** @brief This file contains all command and objects definitions for scripting.
**
** Started on  Sat Jan 25 07:48:41 2003 jfv
** $Id$: tables.c,v 1.54 2008-02-16 12:32:27 thor Exp $
*/
#include "revm.h"

/* The ELF shell module hash tables: returns a revmmod_t */
hash_t		mod_hash;

/* The file hash table : hash the filename and returns a elfshobj_t pointer */
hash_t		file_hash;

/* The Level 1 object hash table : hash the object name and returns a revmL1_t pointer */
hash_t		L1_hash;

/* The Level 2 object hash table list : hash the object name and returns a revmL2_t pointer */
hash_t		elf_L2_hash;	/* For the ELF header fields */
hash_t		sht_L2_hash;	/* For the Sections/SHT objects */
hash_t		pht_L2_hash;	/* For the Program header table objects */
hash_t		sym_L2_hash;	/* For symbol object field */
hash_t		rel_L2_hash;	/* For relocation entry fields */
hash_t		dynsym_L2_hash;	/* For .dynsym objects */
hash_t		dyn_L2_hash;	/* For .dynamic objects */
hash_t		sct_L2_hash;	/* Sections names and data */
hash_t		got_L2_hash;	/* GOT objects */
hash_t          vers_L2_hash;   /* For .gnu.version */
hash_t          verd_L2_hash;   /* For .gnu.version_d */
hash_t          vern_L2_hash;   /* For .gnu.version_r */
hash_t          hashb_L2_hash;  /* For .hash (bucket) */
hash_t          hashc_L2_hash;  /* For .hash (chain) */

/* Color hash */
hash_t          fg_color_hash;
hash_t          bg_color_hash;
hash_t          t_color_hash;

/* Grammar parser hash */
hash_t		parser_hash;

/* Lib path store variable */
char	       elfsh_libpath[BUFSIZ];

/* Hash of instruction lists */
hash_t		instrlists_hash;

/* Hashes for dataflow IN and OUT */
hash_t		inputdf;
hash_t		outputdf;


/**
 * Fill all the Level 1 Objects hash tables 
 */
static void	setup_L1hash()
{
  hash_init(&L1_hash, "L1objects", 29, ASPECT_TYPE_HASH);
  hash_add(&L1_hash, "hdr"     , (void *) revm_create_L1ENT(elfsh_get_hdr,
							  NULL, NULL,
							  &elf_L2_hash,
							  NULL, NULL, NULL, 0));

  hash_add(&L1_hash, "pht"     , (void *) revm_create_L1ENT(elfsh_get_pht,
							  NULL, NULL,
							  &pht_L2_hash,
							  elfsh_get_pht_entry_by_index,
							  NULL, NULL, 
							  sizeof (elfsh_Phdr)));

  hash_add(&L1_hash, "rpht"    , (void *) revm_create_L1ENT(elfsh_get_rpht,
							  NULL, NULL,
							  &pht_L2_hash,
							  elfsh_get_pht_entry_by_index,
							  NULL, NULL, 
							  sizeof (elfsh_Phdr)));


  hash_add(&L1_hash, "symtab"  , (void *) revm_create_L1ENT(elfsh_get_symtab,
							  NULL,
							  elfsh_get_symbol_by_name,
							  &sym_L2_hash,
							  elfsh_get_symbol_by_index,
							  NULL, NULL,
							  sizeof (elfsh_Sym)));

  hash_add(&L1_hash, "dynsym"  , (void *) revm_create_L1ENT(elfsh_get_dynsymtab,
							  NULL,
							  elfsh_get_dynsymbol_by_name,
							  &dynsym_L2_hash,
							  elfsh_get_symbol_by_index,
							  NULL, NULL,
							  sizeof (elfsh_Sym)));

  hash_add(&L1_hash, "got"     , (void *) revm_create_L1ENT(elfsh_get_got,
							  elfsh_get_got_by_idx,  
							  elfsh_get_got_entry_by_name,
							  &got_L2_hash,
							  elfsh_get_got_entry_by_index,
       							  elfsh_get_got_entry,
							  elfsh_set_got_entry,
							  sizeof (eresi_Addr)));


  hash_add(&L1_hash, "dynamic" , (void *) revm_create_L1ENT(elfsh_get_dynamic,
							  NULL, NULL,
							  &dyn_L2_hash,
							  elfsh_get_dynamic_entry_by_index,
							  NULL, NULL,
							  sizeof (elfsh_Dyn)));

  hash_add(&L1_hash, "ctors"   , (void *) revm_create_L1ENT(elfsh_get_ctors,
							  NULL, 
							  elfsh_get_ctors_entry_by_name,
							  NULL,
							  elfsh_get_ctors_entry_by_index,
							  elfsh_get_ctors_entry,
							  elfsh_set_ctors_entry,
							  sizeof (eresi_Addr)));

  hash_add(&L1_hash, "dtors"   , (void *) revm_create_L1ENT(elfsh_get_dtors,
							  NULL, 
							  elfsh_get_dtors_entry_by_name,
							  NULL,
							  elfsh_get_dtors_entry_by_index,
							  elfsh_get_dtors_entry,
							  elfsh_set_dtors_entry,
							  sizeof (eresi_Addr)));
  
  hash_add(&L1_hash, "rel"     , (void *) revm_create_L1ENT(NULL,
							  elfsh_get_reloc, // slot 2
							  elfsh_get_relent_by_name,
							  &rel_L2_hash,
							  elfsh_get_relent_by_index,
							  NULL, NULL,
							  sizeof (elfsh_Rel)));
  
  hash_add(&L1_hash, "sht"     , (void *) revm_create_L1ENT(elfsh_get_sht,
							  NULL,
							  elfsh_get_sht_entry_by_name,
							  &sht_L2_hash,
							  elfsh_get_sht_entry_by_index,
							  NULL, NULL,
							  sizeof (elfsh_Shdr)));

  hash_add(&L1_hash, "rsht"     , (void *) revm_create_L1ENT(elfsh_get_runtime_sht,
							   NULL,
							   elfsh_get_sht_entry_by_name,
							   &sht_L2_hash,
							   elfsh_get_sht_entry_by_index,
							   NULL, NULL,
							   sizeof (elfsh_Shdr)));
  
  hash_add(&L1_hash, "section", (void *) revm_create_L1ENT(elfsh_get_section_list, 
							 NULL,
							 elfsh_get_section_by_nam,
							 &sct_L2_hash,
							 elfsh_get_section_by_idx,
							 NULL, NULL,
							 sizeof (elfshsect_t)));

  hash_add(&L1_hash, "version", (void *) revm_create_L1ENT(elfsh_get_versymtab,
							 NULL,
							 elfsh_get_versym_by_name,
							 &vers_L2_hash,
							 elfsh_get_versym_entry_by_index,
							 NULL, NULL,
							 sizeof (elfsh_Half)));
  
  hash_add(&L1_hash, "verdef", (void *) revm_create_L1ENT(elfsh_get_verdeftab,
							elfsh_get_verdeftab_by_idx,
							NULL,
							&verd_L2_hash,
							elfsh_get_verdef_entry_by_index,
							NULL, NULL,
							sizeof (elfsh_Verdef)));
  
  hash_add(&L1_hash, "verneed", (void *) revm_create_L1ENT(elfsh_get_verneedtab,
							 NULL,
							 elfsh_get_verneed_by_name,
							 &vern_L2_hash,
							 elfsh_get_verneed_entry_by_index, 
							 NULL, NULL,
							 sizeof (elfsh_Verneed)));

  hash_add(&L1_hash, "hashbucket", (void *) revm_create_L1ENT(elfsh_get_hashtable,
							    NULL,
							    elfsh_get_hashbucket_by_name,
							    &hashb_L2_hash,
							    elfsh_get_hashbucket_by_index,
							    NULL, NULL,
							    sizeof (int)));

  hash_add(&L1_hash, "hashchain", (void *) revm_create_L1ENT(elfsh_get_hashtable,
							   NULL,
							   elfsh_get_hashchain_by_name,
							   &hashc_L2_hash,
							   elfsh_get_hashchain_by_index,
							   NULL, NULL,
							   sizeof (int)));
}


/** 
 * Hash table for ELF header fields 
 */
static void	setup_elfhash()
{
  hash_init(&elf_L2_hash, "elfL2objects", 29, ASPECT_TYPE_UNKNOW);
  hash_add(&elf_L2_hash, "magic"        , revm_create_L2ENT(elfsh_get_magic, 
							  elfsh_set_magic, 
							  ASPECT_TYPE_INT,
							  NULL, NULL, 
							  NULL, NULL));
							  
  hash_add(&elf_L2_hash, "class"        , revm_create_L2ENT(elfsh_get_class, 
							  elfsh_set_class, 
							  ASPECT_TYPE_INT,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "type"         , revm_create_L2ENT(elfsh_get_objtype, 
							  elfsh_set_objtype, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "machine"      , revm_create_L2ENT(elfsh_get_arch, 
							  elfsh_set_arch, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "version"      , revm_create_L2ENT(elfsh_get_version, 
							  elfsh_set_version, 
							  ASPECT_TYPE_INT,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "entry"        , revm_create_L2ENT(elfsh_get_entrypoint, 
							  elfsh_set_entrypoint, 
							  ASPECT_TYPE_CADDR,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phoff"        , revm_create_L2ENT(elfsh_get_phtoff, 
							  elfsh_set_phtoff, 
							  ASPECT_TYPE_LONG,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "rphoff"       , revm_create_L2ENT(elfsh_get_rphtoff, 
							  elfsh_set_rphtoff, 
							  ASPECT_TYPE_LONG,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shoff"        , revm_create_L2ENT(elfsh_get_shtoff, 
							  elfsh_set_shtoff, 
							  ASPECT_TYPE_LONG,
							  NULL, NULL, NULL, NULL));
  hash_add(&elf_L2_hash, "flags"        , revm_create_L2ENT(elfsh_get_flags, 
							  elfsh_set_flags, 
							  ASPECT_TYPE_INT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "ehsize"       , revm_create_L2ENT(elfsh_get_ehsize,
							  elfsh_set_ehsize, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phentsize"    , revm_create_L2ENT(elfsh_get_phentsize,
							  elfsh_set_phentsize, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shentsize"    , revm_create_L2ENT(elfsh_get_shentsize,
							  elfsh_set_shentsize, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phnum"        , revm_create_L2ENT(elfsh_get_phtnbr,
							  elfsh_set_phtnbr, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shnum"        , revm_create_L2ENT(elfsh_get_shtnbr,
							  elfsh_set_shtnbr, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shstrndx"     , revm_create_L2ENT(elfsh_get_shstrtab_index,
							  elfsh_set_shstrtab_index,
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_pageexec" , revm_create_L2ENT(elfsh_get_pax_pageexec, 
							  elfsh_set_pax_pageexec, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_emultramp", revm_create_L2ENT(elfsh_get_pax_emultramp, 
							  elfsh_set_pax_emultramp, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_mprotect" , revm_create_L2ENT(elfsh_get_pax_mprotect, 
							  elfsh_set_pax_mprotect, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_randmmap" , revm_create_L2ENT(elfsh_get_pax_randmmap, 
							  elfsh_set_pax_randmmap, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_randexec" , revm_create_L2ENT(elfsh_get_pax_randexec, 
							  elfsh_set_pax_randexec, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_segmexec" , revm_create_L2ENT(elfsh_get_pax_segmexec, 
							  elfsh_set_pax_segmexec, 
							  ASPECT_TYPE_SHORT,
							  NULL, NULL, 
							  NULL, NULL));
}



/** 
 * Hash table for SHT fields 
 */
static void	setup_shthash()
{
  hash_init(&sht_L2_hash, "shtL2objects", 29, ASPECT_TYPE_UNKNOW);
  hash_add(&sht_L2_hash, "type"     , revm_create_L2ENT(elfsh_get_section_type, 
						      elfsh_set_section_type, 
						      ASPECT_TYPE_SHORT, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "addr"     , revm_create_L2ENT(elfsh_get_section_addr, 
						      elfsh_set_section_addr, 
						      ASPECT_TYPE_LONG, NULL, 
						      NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "offset"   , revm_create_L2ENT(elfsh_get_section_foffset, 
						      elfsh_set_section_foffset, 
						      ASPECT_TYPE_LONG, NULL,
						      NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "size"     , revm_create_L2ENT(elfsh_get_section_size, 
						      elfsh_set_section_size, 
						      ASPECT_TYPE_LONG, NULL,
						      NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "link"     , revm_create_L2ENT(elfsh_get_section_link, 
						      elfsh_set_section_link, 
						      ASPECT_TYPE_SHORT, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "info"     , revm_create_L2ENT(elfsh_get_section_info, 
						      elfsh_set_section_info, 
						      ASPECT_TYPE_SHORT, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "align"    , revm_create_L2ENT(elfsh_get_section_align, 
						      elfsh_set_section_align, 
						      ASPECT_TYPE_LONG, NULL,
						      NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "entsize"  , revm_create_L2ENT(elfsh_get_section_entsize, 
						      elfsh_set_section_entsize,
						      ASPECT_TYPE_LONG, NULL,
						      NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "a"  , revm_create_L2ENT(elfsh_get_section_allocflag, 
						elfsh_set_section_allocflag, 
						ASPECT_TYPE_LONG, NULL, 
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "w"  , revm_create_L2ENT(elfsh_get_section_writableflag, 
						elfsh_set_section_writableflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "x"  , revm_create_L2ENT(elfsh_get_section_execflag, 
						elfsh_set_section_execflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "s"  , revm_create_L2ENT(elfsh_get_section_strflag, 
						elfsh_set_section_strflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "m"  , revm_create_L2ENT(elfsh_get_section_mergeableflag, 
						elfsh_set_section_mergeableflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "l"  , revm_create_L2ENT(elfsh_get_section_linkflag, 
						elfsh_set_section_linkflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "o"  , revm_create_L2ENT(elfsh_get_section_orderflag, 
						elfsh_set_section_orderflag, 
						ASPECT_TYPE_LONG, NULL,
						NULL, NULL, NULL));
}

/** 
 * Hash table for PHT fields 
 */
static void	setup_phthash()
{
  hash_init(&pht_L2_hash, "phtL2objects", 29, ASPECT_TYPE_UNKNOW);
  hash_add(&pht_L2_hash, "type"  , revm_create_L2ENT(elfsh_get_segment_type, 
						   elfsh_set_segment_type, 
						   ASPECT_TYPE_SHORT, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "offset", revm_create_L2ENT(elfsh_get_segment_offset, 
						   elfsh_set_segment_offset, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "vaddr" , revm_create_L2ENT(elfsh_get_segment_vaddr, 
						   elfsh_set_segment_vaddr, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "paddr" , revm_create_L2ENT(elfsh_get_segment_paddr, 
						   elfsh_set_segment_paddr, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "filesz", revm_create_L2ENT(elfsh_get_segment_filesz, 
						   elfsh_set_segment_filesz, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "memsz" , revm_create_L2ENT(elfsh_get_segment_memsz, 
						   elfsh_set_segment_memsz, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "flags" , revm_create_L2ENT(elfsh_get_segment_flags, 
						   elfsh_set_segment_flags, 
						   ASPECT_TYPE_SHORT, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "align" , revm_create_L2ENT(elfsh_get_segment_align, 
						   elfsh_set_segment_align, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
}


/**
 * Hash table for symbol table 
 */
static void	setup_symhash()
{
  hash_init(&sym_L2_hash, "symL2objects", 23, ASPECT_TYPE_UNKNOW);

  hash_add(&sym_L2_hash, "name" , revm_create_L2ENT(NULL, NULL, ASPECT_TYPE_STR, 
						  elfsh_get_symbol_name,
						  elfsh_set_symbol_name,
						  NULL, NULL));

  hash_add(&sym_L2_hash, "value", revm_create_L2ENT(elfsh_get_symbol_value, 
						  elfsh_set_symbol_value, 
						  ASPECT_TYPE_LONG, NULL,
						  NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "size" , revm_create_L2ENT(elfsh_get_symbol_size, 
						  elfsh_set_symbol_size, 
						  ASPECT_TYPE_LONG, NULL,
						  NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "bind" , revm_create_L2ENT(elfsh_get_symbol_bind, 
						  elfsh_set_symbol_bind, 
						  ASPECT_TYPE_BYTE, NULL, 
						  NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "type" , revm_create_L2ENT(elfsh_get_symbol_type, 
						  elfsh_set_symbol_type, 
						  ASPECT_TYPE_BYTE, NULL, 
						  NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "other", revm_create_L2ENT(elfsh_get_symbol_visibility, 
						  elfsh_set_symbol_visibility, 
						  ASPECT_TYPE_BYTE, NULL, 
						  NULL, NULL, NULL));

  hash_add(&sym_L2_hash, "link", revm_create_L2ENT(elfsh_get_symbol_link, 
						 elfsh_set_symbol_link, 
						 ASPECT_TYPE_SHORT, NULL, 
						 NULL, NULL, NULL));
}



/** 
 * Hash table for dynamic symbol table 
 */
static void	setup_dynsymhash()
{
  hash_init(&dynsym_L2_hash, "dynsymL2objects", 23, ASPECT_TYPE_UNKNOW);
  hash_add(&dynsym_L2_hash, "name" , revm_create_L2ENT(NULL, NULL, ASPECT_TYPE_STR,
						     elfsh_get_dynsymbol_name,
						     elfsh_set_dynsymbol_name,
						     NULL, NULL));
  hash_add(&dynsym_L2_hash, "value", revm_create_L2ENT(elfsh_get_symbol_value, 
						     elfsh_set_symbol_value, 
						     ASPECT_TYPE_LONG, NULL,
						     NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "size" , revm_create_L2ENT(elfsh_get_symbol_size, 
						     elfsh_set_symbol_size, 
						     ASPECT_TYPE_LONG, NULL,
						     NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "bind" , revm_create_L2ENT(elfsh_get_symbol_bind, 
						     elfsh_set_symbol_bind, 
						     ASPECT_TYPE_BYTE, NULL, 
						     NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "type" , revm_create_L2ENT(elfsh_get_symbol_type, 
						     elfsh_set_symbol_type, 
						     ASPECT_TYPE_BYTE, NULL, 
						     NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "other", revm_create_L2ENT(elfsh_get_symbol_visibility, 
						     elfsh_set_symbol_visibility, 
						     ASPECT_TYPE_BYTE, NULL, 
						     NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "link", revm_create_L2ENT(elfsh_get_symbol_link, 
						    elfsh_set_symbol_link, 
						    ASPECT_TYPE_SHORT, NULL, 
						    NULL, NULL, NULL));
}

/** 
 * Hash table for .rel sections 
 */
static void	setup_relhash()
{
  hash_init(&rel_L2_hash, "relL2objects", 23, ASPECT_TYPE_UNKNOW);
  hash_add(&rel_L2_hash, "type"  , revm_create_L2ENT(elfsh_get_reltype, 
						   elfsh_set_reltype, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "sym"   , revm_create_L2ENT(elfsh_get_relsym, 
						   elfsh_set_relsym, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "offset", revm_create_L2ENT(elfsh_get_reloffset, 
						   elfsh_set_reloffset, 
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "addend", revm_create_L2ENT(elfsh_get_reladdend, 
						   elfsh_set_reladdend,
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
}

/** 
 * Hash table for .dynamic section 
 */
static void	setup_dynhash()
{
  hash_init(&dyn_L2_hash, "dynL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&dyn_L2_hash, "val", revm_create_L2ENT(elfsh_get_dynentry_val, 
						elfsh_set_dynentry_val, 
						ASPECT_TYPE_LONG, NULL, 
						NULL, NULL, NULL));
  hash_add(&dyn_L2_hash, "tag", revm_create_L2ENT(elfsh_get_dynentry_type, 
						elfsh_set_dynentry_type, 
						ASPECT_TYPE_LONG, NULL, 
						NULL, NULL, NULL));
}



/** 
 * Hash tables for GOT L2 objects : UNIMPLEMENTED for now, only 
 * the value of the entry can be changed in this version of 
 * the ELF shell using the got[name|idx] format without L2 field.
 */
static void	setup_gothash()
{
  
  hash_init(&got_L2_hash, "gotL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&got_L2_hash, "val" , revm_create_L2ENT(elfsh_get_got_val, elfsh_set_got_val,
						 ASPECT_TYPE_LONG, NULL,
						 NULL, NULL, NULL));
  //hash_add(&got_L2_hash, "idx" , revm_create_L2ENT());
  hash_add(&got_L2_hash, "addr", revm_create_L2ENT(elfsh_get_got_addr, elfsh_set_got_addr,
						 ASPECT_TYPE_LONG, NULL,
						 NULL, NULL, NULL));
}



/** 
 * Hash tables for sections data 
 */
static void	setup_scthash()
{
  hash_init(&sct_L2_hash, "sectsL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&sct_L2_hash, "name", revm_create_L2ENT(NULL, NULL, ASPECT_TYPE_STR,
						 elfsh_get_section_name,
						 elfsh_set_section_name,
						 NULL, NULL));
  
  hash_add(&sct_L2_hash, "raw", revm_create_L2ENT(NULL, NULL, 
						ASPECT_TYPE_RAW,
						NULL, NULL, 
						elfsh_get_section_data,
						elfsh_write_section_data));
}



/** 
 * Hash table for versions sections
 */
static void   setup_vershash()
{
  
  /* Version symbols */
  hash_init(&vers_L2_hash, "versionL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&vers_L2_hash, "value", revm_create_L2ENT(elfsh_get_versym_val,
						   elfsh_set_versym_val,
						   ASPECT_TYPE_SHORT, NULL,
						   NULL, NULL, NULL));
  
  /* Version def */
  hash_init(&verd_L2_hash, "verdefsL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&verd_L2_hash, "index", revm_create_L2ENT(elfsh_get_verdef_ndx,
						   elfsh_set_verdef_ndx,
						   ASPECT_TYPE_SHORT, NULL,
						   NULL, NULL, NULL));
   hash_add(&verd_L2_hash, "count", revm_create_L2ENT(elfsh_get_verdef_cnt,
						    elfsh_set_verdef_cnt,
						    ASPECT_TYPE_SHORT, NULL,
						    NULL, NULL, NULL));
   hash_add(&verd_L2_hash, "hash", revm_create_L2ENT(elfsh_get_verdef_hash, 
						   elfsh_set_verdef_hash, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   hash_add(&verd_L2_hash, "flag", revm_create_L2ENT(elfsh_get_verdef_flags, 
						   elfsh_set_verdef_flags, 
						   ASPECT_TYPE_SHORT, NULL, 
						   NULL, NULL, NULL));
   hash_add(&verd_L2_hash, "aux", revm_create_L2ENT(elfsh_get_verdef_aux, 
						  elfsh_set_verdef_aux, 
						  ASPECT_TYPE_LONG, NULL, 
						  NULL, NULL, NULL));


   /* Child & parent */
   hash_add(&verd_L2_hash, "next", revm_create_L2ENT(elfsh_get_verdef_next, 
						   elfsh_set_verdef_next, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   /* Child */
   hash_add(&verd_L2_hash, "name", revm_create_L2ENT(elfsh_get_verdef_cname, 
						   elfsh_set_verdef_cname, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   
   /* Version need */
   hash_init(&vern_L2_hash, "verneedL2objects", 11, ASPECT_TYPE_UNKNOW);
   hash_add(&vern_L2_hash, "index", revm_create_L2ENT(elfsh_get_verneed_ndx, 
						    elfsh_set_verneed_ndx, 
						    ASPECT_TYPE_SHORT, NULL, 
						    NULL, NULL, NULL));  
   
   hash_add(&vern_L2_hash, "name", revm_create_L2ENT(elfsh_get_verneed_name, 
						   elfsh_set_verneed_name, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   
   hash_add(&vern_L2_hash, "hash", revm_create_L2ENT(elfsh_get_verneed_hash,
						   elfsh_set_verneed_hash,
						   ASPECT_TYPE_LONG, NULL,
						   NULL, NULL, NULL));
   hash_add(&vern_L2_hash, "flag", revm_create_L2ENT(elfsh_get_verneed_flags, 
						   elfsh_set_verneed_flags, 
						   ASPECT_TYPE_SHORT, NULL, 
						   NULL, NULL, NULL));
   /* child && parent */
   hash_add(&vern_L2_hash, "next", revm_create_L2ENT(elfsh_get_verneed_next, 
						   elfsh_set_verneed_next, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   /* parent */
   hash_add(&vern_L2_hash, "file", revm_create_L2ENT(elfsh_get_verneed_file, 
						   elfsh_set_verneed_file, 
						   ASPECT_TYPE_LONG, NULL, 
						   NULL, NULL, NULL));
   hash_add(&vern_L2_hash, "count", revm_create_L2ENT(elfsh_get_verneed_cnt, 
						    elfsh_set_verneed_cnt, 
						    ASPECT_TYPE_SHORT, NULL, 
						    NULL, NULL, NULL));
   hash_add(&vern_L2_hash, "aux", revm_create_L2ENT(elfsh_get_verneed_aux, 
						  elfsh_set_verneed_aux, 
						  ASPECT_TYPE_LONG, NULL, 
						  NULL, NULL, NULL));
}


/**
 * TO COMMENT
 */
static void   setup_hashhash()
{

  /* Hash bucket */
  hash_init(&hashb_L2_hash, "hashbucketsL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&hashb_L2_hash, "value", revm_create_L2ENT(elfsh_get_hashbucket_value, 
						    elfsh_set_hashbucket_value, 
						    ASPECT_TYPE_INT, NULL, 
						    NULL, NULL, NULL));  
  hash_add(&hashb_L2_hash, "nbucket", revm_create_L2ENT(elfsh_get_hashbucket_num, 
						      elfsh_set_hashbucket_num, 
						      ASPECT_TYPE_INT, NULL, 
						      NULL, NULL, NULL)); 
  /* Hash chain */
  hash_init(&hashc_L2_hash, "hashchainL2objects", 11, ASPECT_TYPE_UNKNOW);
  hash_add(&hashc_L2_hash, "value", revm_create_L2ENT(elfsh_get_hashchain_value, 
						    elfsh_set_hashchain_value, 
						    ASPECT_TYPE_INT, NULL, 
						    NULL, NULL, NULL));  
  hash_add(&hashc_L2_hash, "nchain", revm_create_L2ENT(elfsh_get_hashchain_num, 
						     elfsh_set_hashchain_num, 
						     ASPECT_TYPE_INT, NULL, 
						     NULL, NULL, NULL));   
}




/**
 * Now comes Level 2 objects hash functions 
 */
static void	setup_L2hash()
{
  setup_elfhash();
  setup_shthash();
  setup_phthash();
  setup_symhash();
  setup_relhash();
  setup_dynsymhash();
  setup_dynhash();
  setup_scthash();
  setup_gothash(); 
  setup_vershash();
  setup_hashhash();
}


/**
 * Mix default library path with LD_LIBRARY_PATH variable 
 */
static char	*get_libpath()
{
  int		len;
  char		*ldenv;

  /* Set default value */
  strncpy(elfsh_libpath, ERESI_LIBPATH, BUFSIZ);
  elfsh_libpath[BUFSIZ - 1] = '\0';

  len = strlen(elfsh_libpath);

  ldenv = getenv("LD_LIBRARY_PATH");

  /* Check if we use LD_LIBRARY_PATH */
  if (ldenv != NULL && len+strlen(ldenv)+2 < BUFSIZ)
    {
      /* We separate each path with ; */
      if (ldenv[0] != ';' && elfsh_libpath[len - 1] != ';')
	  strcat(elfsh_libpath, ";");
      else if (ldenv[0] == ';' && elfsh_libpath[len - 1] == ';')
	  ldenv++;

      strcat(elfsh_libpath, ldenv);
    }

  return elfsh_libpath;
}


/**
 * Setup variables hash :
 * - Initialize $_ (last result variable) to 0 
 * - Initialize $! (last openered file) to 0
 */
static void	setup_varshash()
{
  revmobj_t	*f;
  revmexpr_t	*expr;
  char		*str;

  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0);
  expr = revm_expr_create_from_object(f, REVM_VAR_RESULT, 1);
  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0);
  expr = revm_expr_create_from_object(f, REVM_VAR_LOAD, 1);
  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0xFFFFFFFF);
  expr = revm_expr_create_from_object(f, REVM_VAR_ERROR, 1);
  f = revm_create_IMMEDSTR(1, ERESI_EXTSHELL);
  expr = revm_expr_create_from_object(f, REVM_VAR_SHELL, 1);
  f = revm_create_IMMEDSTR(1, ERESI_EDITOR);
  expr = revm_expr_create_from_object(f, REVM_VAR_EDITOR, 1);
  str = get_libpath();
  f = revm_create_IMMEDSTR(1, str);
  expr = revm_expr_create_from_object(f, REVM_VAR_LIBPATH, 1);
  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, ERESI_SLOG);
  expr = revm_expr_create_from_object(f, REVM_VAR_STRIPLOG, 1);
  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0);
  expr = revm_expr_create_from_object(f, REVM_VAR_ESHLEVEL, 1);
}


/** 
 * Setup default grammar
 * XXX: This function is temporary and we not remain in the new type system 
 */
void setup_grammar()
{

  /* Default grammar rules */
#define LOOKUP5_IDX  "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]][%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_A    "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^:]:%41[^%%]%%%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_B    "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^:]:%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_C    "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"REVM_SEP"%41s"
#define LOOKUP3_IDX  "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"
#define LOOKUP3      "%41[^"REVM_SEP"]"REVM_SEP"%41[^"REVM_SEP"]"REVM_SEP"%41s"
#define LOOKUP_VECT  "$vector[%41[^]]]"
#define LOOKUP_HASH  "$hash[%41[^]]]"
  //#define LOOKUP_HASH1 "$hash[%41[^]]]"REVM_SEP"%41s"
#define LOOKUP_LIST  "$list[%41[^]]]"

  hash_add(&parser_hash, LOOKUP5_IDX, parse_lookup5_index);
  hash_add(&parser_hash, LOOKUP4_A, parse_lookup4);
  hash_add(&parser_hash, LOOKUP4_B, parse_lookup4);
  hash_add(&parser_hash, LOOKUP4_C, parse_lookup4);
  hash_add(&parser_hash, LOOKUP3_IDX, parse_lookup3_index);
  hash_add(&parser_hash, LOOKUP3, parse_lookup3);
  hash_add(&parser_hash, LOOKUP_VECT, parse_vector);
  hash_add(&parser_hash, LOOKUP_HASH, parse_hash);
  //hash_add(&parser_hash, LOOKUP_HASH1, parse_hash_field);
  hash_add(&parser_hash, LOOKUP_LIST, parse_list);
}



/** 
 * Setup color table 
 */
void setup_color()
{
  hash_add(&fg_color_hash, "none"   , (void *) COLOR_RESET);
  hash_add(&fg_color_hash, "black"  , (void *) COLOR_FG_BLACK);
  hash_add(&fg_color_hash, "red"    , (void *) COLOR_FG_RED);
  hash_add(&fg_color_hash, "green"  , (void *) COLOR_FG_GREEN);
  hash_add(&fg_color_hash, "yellow" , (void *) COLOR_FG_YELLOW);
  hash_add(&fg_color_hash, "blue"   , (void *) COLOR_FG_BLUE);
  hash_add(&fg_color_hash, "magenta", (void *) COLOR_FG_MAGENTA);
  hash_add(&fg_color_hash, "cyan"   , (void *) COLOR_FG_CYAN);
  hash_add(&fg_color_hash, "white"  , (void *) COLOR_FG_WHITE);

  hash_add(&bg_color_hash, "none"   , (void *) COLOR_RESET);
  hash_add(&bg_color_hash, "black"  , (void *) COLOR_BG_BLACK);
  hash_add(&bg_color_hash, "red"    , (void *) COLOR_BG_RED);
  hash_add(&bg_color_hash, "green"  , (void *) COLOR_BG_GREEN);
  hash_add(&bg_color_hash, "yellow" , (void *) COLOR_BG_YELLOW);
  hash_add(&bg_color_hash, "blue"   , (void *) COLOR_BG_BLUE);
  hash_add(&bg_color_hash, "magenta", (void *) COLOR_BG_MAGENTA);
  hash_add(&bg_color_hash, "cyan"   , (void *) COLOR_BG_CYAN);
  hash_add(&bg_color_hash, "white"  , (void *) COLOR_BG_WHITE);
}


/**
 * TOCOMMENT
 */
void setup_color_type()
{
  hash_add(&t_color_hash, "string"     , (void *) revm_colorblank());
  hash_add(&t_color_hash, "fieldstring", (void *) revm_colorblank()); 
  hash_add(&t_color_hash, "typestring" , (void *) revm_colorblank());
  hash_add(&t_color_hash, "address"    , (void *) revm_colorblank());
  hash_add(&t_color_hash, "number"     , (void *) revm_colorblank());
  hash_add(&t_color_hash, "endstring"  , (void *) revm_colorblank());
  hash_add(&t_color_hash, "warnstring" , (void *) revm_colorblank());
  hash_add(&t_color_hash, "pspecial"   , (void *) revm_colorblank());
  hash_add(&t_color_hash, "psname"     , (void *) revm_colorblank());
  hash_add(&t_color_hash, "pversion"   , (void *) revm_colorblank());
  hash_add(&t_color_hash, "prelease"   , (void *) revm_colorblank());
  hash_add(&t_color_hash, "pedition"   , (void *) revm_colorblank());
  hash_add(&t_color_hash, "instr"      , (void *) revm_colorblank());
  hash_add(&t_color_hash, "function"   , (void *) revm_colorblank());
  hash_add(&t_color_hash, "filename"   , (void *) revm_colorblank());
}




/** 
 * Setup all hash tables 
 */
void		revm_tables_setup()
{
  static int	done = 0;

  if (done)
    return;
  done = 1;

  hash_init(&file_hash        , "files"      , 251, ASPECT_TYPE_UNKNOW);
  hash_init(&mod_hash         , "modules"    , 51, ASPECT_TYPE_UNKNOW);
  hash_init(&fg_color_hash    , "fgcolors"   , 13, ASPECT_TYPE_UNKNOW);
  hash_init(&bg_color_hash    , "bgcolors"   , 13, ASPECT_TYPE_UNKNOW);
  hash_init(&t_color_hash     , "tcolors"    , 11, ASPECT_TYPE_UNKNOW);
  hash_init(&world.shared_hash, "sharedfiles", 11, ASPECT_TYPE_UNKNOW);
  hash_init(&instrlists_hash  , "instrlists" , 51, ASPECT_TYPE_LIST);
  hash_init(&inputdf          , "idflists"   , 51, ASPECT_TYPE_LIST);
  hash_init(&outputdf         , "odflists"   , 51, ASPECT_TYPE_LIST);
  hash_init(&parser_hash      , "parsers"    , 11, ASPECT_TYPE_CADDR);
  
  if (world.cmd_init)
    (*world.cmd_init)();
  else
    fprintf(stderr, "world.cmd_init = NULL !!!! \n");

  setup_varshash();
  setup_L1hash();
  setup_L2hash();
  setup_color();
  setup_color_type();
  setup_grammar();
  aspect_init();
}
