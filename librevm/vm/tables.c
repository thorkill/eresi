/*
** tables.c for elfsh
**
** This file contains all command and objects definitions for scripting
**
** Started on  Sat Jan 25 07:48:41 2003 jfv
**
** $Id: tables.c,v 1.47 2007-08-03 18:05:03 may Exp $
**
*/
#include "revm.h"



/* Hash of instruction lists */
hash_t		instrlists_hash;

/* Labels hash */
hash_t		labels_hash[REVM_MAXSRCNEST];

/* Variables hash */
hash_t		vars_hash;

/* ERESI expressions hash and expressions type hash */
hash_t		exprs_hash;
hash_t		exprtypes_hash;

/* The command hash table : hash the command name and returns a revmcmd_t */
hash_t		cmd_hash;

/* The constants hash tables : ascii defines from elf.h and revm.h */
hash_t		const_hash;

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

/* Trace hash */
hash_t		traces_cmd_hash;

/* Lib path store variable */
char	       elfsh_libpath[BUFSIZ];



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
							  sizeof (elfsh_Addr)));


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
							  sizeof (elfsh_Addr)));

  hash_add(&L1_hash, "dtors"   , (void *) revm_create_L1ENT(elfsh_get_dtors,
							  NULL, 
							  elfsh_get_dtors_entry_by_name,
							  NULL,
							  elfsh_get_dtors_entry_by_index,
							  elfsh_get_dtors_entry,
							  elfsh_set_dtors_entry,
							  sizeof (elfsh_Addr)));
  
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
						      0, NULL, NULL, NULL, NULL));
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
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "info"     , revm_create_L2ENT(elfsh_get_section_info, 
						      elfsh_set_section_info, 
						      0, NULL, NULL, NULL, NULL));
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
						   0, NULL, NULL, NULL, NULL));
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
						   0, NULL, NULL, NULL, NULL));
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
 * Setup the command hash table 
 */
static void	setup_cmdhash()
{
  typeinfo_t	*typeinfo;
  unsigned int	typenbr;
  unsigned int	index;

  hash_init(&cmd_hash, "commands", 101, ASPECT_TYPE_UNKNOW);

  /* Interactive mode / Scripting mode commands */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    {
      revm_command_add(CMD_LOAD      , (void *) cmd_load     , (void *) revm_getoption, 0, HLP_LOAD);
      revm_command_add(CMD_UNLOAD    , (void *) cmd_unload   , (void *) revm_getoption   , 0, HLP_UNLOAD);
      revm_command_add(CMD_SAVE      , (void *) cmd_save     , (void *) revm_getoption   , 1, HLP_SAVE);
      revm_command_add(CMD_SWITCH    , (void *) cmd_doswitch , (void *) revm_getoption   , 0, HLP_SWITCH);
      revm_command_add(CMD_METACMD   , (void *) cmd_meta     , (void *) NULL           , 0, HLP_METACMD);
      revm_command_add(CMD_QUIT      , (void *) cmd_quit     , (void *) NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_QUIT2     , (void *) cmd_quit     , (void *) NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_LIST      , (void *) cmd_dolist   , (void *) NULL           , 0, HLP_LIST);
      revm_command_add(CMD_LIST2     , (void *) cmd_dolist   , (void *) NULL           , 0, HLP_LIST);
      revm_command_add(CMD_STOP      , (void *) cmd_stop     , (void *) NULL           , 0, HLP_STOP);
      revm_command_add(CMD_WORKSPACE , (void *) cmd_workspace, (void *) revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_WORKSPACE2, (void *) cmd_workspace, (void *) revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_WORKSPACE2, (void *) cmd_workspace, (void *) revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_VECTORS   , (void *) cmd_vectors  , (void *) revm_getvarparams, 0, HLP_VECTORS);
      revm_command_add(CMD_TABLES    , (void *) cmd_tables   , (void *) revm_getvarparams, 0, HLP_TABLES);
      revm_command_add(CMD_LISTS     , (void *) cmd_lists    , (void *) revm_getvarparams, 0, HLP_LISTS);
      revm_command_add(CMD_EMPTY     , (void *) cmd_empty    , (void *) revm_getvarparams, 0, HLP_EMPTY);

      revm_command_add(CMD_DEFINE    , (void *) cmd_define   , (void *) revm_getoption2   , 0, HLP_DEFINE);
      revm_command_add(CMD_UNDEF     , (void *) cmd_undef    , (void *) revm_getoption    , 0, HLP_UNDEF);
    }

  /* Command line only commands */
  if (world.state.revm_mode == REVM_STATE_CMDLINE)
    {
      revm_command_add(CMD_BINFILE_W, (void *) NULL, (void *) revm_getoutput, 0, "");
      revm_command_add(CMD_BINFILE_R, (void *) NULL, (void *) revm_getinput , 0, "");
    }

  /* General purpose command */
  revm_command_add(CMD_MODLOAD , (void *) cmd_modload  , (void *) revm_getoption   , 0, HLP_MODLOAD);
  revm_command_add(CMD_MODULOAD, (void *) cmd_modunload, (void *) revm_getoption   , 0, HLP_MODULOAD);
  revm_command_add(CMD_DISASM  , (void *) cmd_disasm  , (void *) revm_getdisasm    , 1, HLP_DISASM);
  revm_command_add(CMD_DISASM2 , (void *) cmd_disasm  , (void *) revm_getdisasm    , 1, HLP_DISASM);
  revm_command_add(CMD_HEXA    , (void *) cmd_disasm  , (void *) revm_gethexa      , 1, HLP_HEXA);
  revm_command_add(CMD_HEXA2   , (void *) cmd_disasm  , (void *) revm_gethexa      , 1, HLP_HEXA);
  revm_command_add(CMD_PRINT   , (void *) cmd_print   , (void *) revm_getvarparams , 0, HLP_PRINT);
  revm_command_add(CMD_EXEC    , (void *) cmd_exec    , (void *) revm_getvarparams , 0, HLP_EXEC);
  revm_command_add(CMD_EDIT    , (void *) cmd_edit    , (void *) revm_getoption    , 0, HLP_EDIT);
  revm_command_add(CMD_PHT     , (void *) cmd_pht     , (void *) revm_getregxoption, 1, HLP_PHT);
  revm_command_add(CMD_GOT     , (void *) cmd_got     , (void *) revm_getregxoption, 1, HLP_GOT);
  revm_command_add(CMD_CTORS   , (void *) cmd_ctors   , (void *) revm_getregxoption, 1, HLP_CTORS);
  revm_command_add(CMD_DTORS   , (void *) cmd_dtors   , (void *) revm_getregxoption, 1, HLP_DTORS);
  revm_command_add(CMD_NOTE    , (void *) cmd_notes   , (void *) revm_getregxoption, 1, HLP_NOTES);
  revm_command_add(CMD_COREINFO, (void *) cmd_coreinfo, (void *) NULL	          , 1, HLP_COREINFO);
  revm_command_add(CMD_SYM     , (void *) cmd_sym     , (void *) revm_getregxoption, 1, HLP_SYM);
  revm_command_add(CMD_DYNAMIC , (void *) cmd_dyn     , (void *) revm_getregxoption, 1, HLP_DYNAMIC);
  revm_command_add(CMD_DYNSYM  , (void *) cmd_dynsym  , (void *) revm_getregxoption, 1, HLP_DYNSYM);
  revm_command_add(CMD_SHT     , (void *) cmd_sht     , (void *) revm_getregxoption, 1, HLP_SHT);
  revm_command_add(CMD_DEBUG   , (void *) cmd_debug   , (void *) revm_getvarparams , 0, HLP_DEBUG);
  revm_command_add(CMD_REL     , (void *) cmd_rel     , (void *) revm_getregxoption, 1, HLP_REL);
  revm_command_add(CMD_COMMENT , (void *) cmd_comments, (void *) revm_getregxoption, 1, HLP_COMMENT);
  revm_command_add(CMD_PHT2    , (void *) cmd_pht     , (void *) revm_getregxoption, 1, HLP_PHT);
  revm_command_add(CMD_GOT2    , (void *) cmd_got     , (void *) revm_getregxoption, 1, HLP_GOT);
  revm_command_add(CMD_CTORS2  , (void *) cmd_ctors   , (void *) revm_getregxoption, 1, HLP_CTORS);
  revm_command_add(CMD_DTORS2  , (void *) cmd_dtors   , (void *) revm_getregxoption, 1, HLP_DTORS);
  revm_command_add(CMD_NOTE2   , (void *) cmd_notes   , (void *) revm_getregxoption, 1, HLP_NOTES);
  revm_command_add(CMD_SYM2    , (void *) cmd_sym     , (void *) revm_getregxoption, 1, HLP_SYM);
  revm_command_add(CMD_DYNAMIC2, (void *) cmd_dyn     , (void *) revm_getregxoption, 1, HLP_DYNAMIC);
  revm_command_add(CMD_DYNSYM2 , (void *) cmd_dynsym  , (void *) revm_getregxoption, 1, HLP_DYNSYM);
  revm_command_add(CMD_SHT2    , (void *) cmd_sht     , (void *) revm_getregxoption, 1, HLP_SHT);
  revm_command_add(CMD_REL2    , (void *) cmd_rel     , (void *) revm_getregxoption, 1, HLP_REL);
  revm_command_add(CMD_COMMENT2, (void *) cmd_comments, (void *) revm_getregxoption, 1, HLP_COMMENT);
  revm_command_add(CMD_SET     , (void *) cmd_set     , (void *) revm_getoption2   , 0, HLP_SET);
  revm_command_add(CMD_ADD     , (void *) cmd_add     , (void *) revm_getoption2   , 0, HLP_ADD);
  revm_command_add(CMD_SUB     , (void *) cmd_sub     , (void *) revm_getoption2   , 0, HLP_SUB);
  revm_command_add(CMD_MUL     , (void *) cmd_mul     , (void *) revm_getoption2   , 0, HLP_MUL);
  revm_command_add(CMD_DIV     , (void *) cmd_div     , (void *) revm_getoption2   , 0, HLP_DIV);
  revm_command_add(CMD_MOD     , (void *) cmd_set     , (void *) revm_getoption2   , 0, HLP_MOD);
  revm_command_add(CMD_INFO    , (void *) cmd_info    , (void *) NULL            , 0, HLP_INFO);
  revm_command_add(CMD_FIXUP   , (void *) cmd_fixup   , (void *) NULL            , 1, HLP_FIXUP);
  revm_command_add(CMD_ELF2    , (void *) cmd_elf     , (void *) NULL            , 1, HLP_ELF);
  revm_command_add(CMD_INTERP2 , (void *) cmd_interp  , (void *) NULL            , 1, HLP_INTERP);
  revm_command_add(CMD_ELF     , (void *) cmd_elf     , (void *) NULL            , 1, HLP_ELF);
  revm_command_add(CMD_INTERP  , (void *) cmd_interp  , (void *) NULL            , 1, HLP_INTERP);
  revm_command_add(CMD_HELP    , (void *) cmd_help    , (void *) revm_getvarparams , 0, HLP_HELP);
  revm_command_add(CMD_MODHELP , (void *) cmd_modhelp , (void *) revm_getoption    , 0, HLP_MODHELP);
  revm_command_add(CMD_QUIET   , (void *) cmd_quiet   , (void *) NULL            , 0, HLP_QUIET);
  revm_command_add(CMD_QUIET2  , (void *) cmd_quiet   , (void *) NULL            , 0, HLP_QUIET);
  revm_command_add(CMD_FORCE   , (void *) cmd_force   , (void *) NULL            , 0, HLP_FORCE);
  revm_command_add(CMD_VERB    , (void *) cmd_verb    , (void *) NULL            , 0, HLP_VERB);
  revm_command_add(CMD_VERB2   , (void *) cmd_verb    , (void *) NULL            , 0, HLP_VERB);
  revm_command_add(CMD_SORT    , (void *) cmd_sort    , (void *) revm_getoption    , 0, HLP_SORT);
  revm_command_add(CMD_SORT2   , (void *) cmd_sort    , (void *) revm_getoption    , 0, HLP_SORT);
  revm_command_add(CMD_ALL	, (void *) cmd_glregx  , (void *) revm_getoption    , 0, HLP_ALL);
  revm_command_add(CMD_ALL2	, (void *) cmd_glregx  , (void *) revm_getoption    , 0, HLP_ALL);
  revm_command_add(CMD_ALERT	, (void *) cmd_alert   , (void *) revm_getoption    , 0, HLP_ALERT);
  revm_command_add(CMD_FINDREL , (void *) cmd_findrel , (void *) NULL            , 1, HLP_FINDREL);
  revm_command_add(CMD_WRITE	, (void *) cmd_write   , (void *) revm_getvarparams , 1, HLP_WRITE);
  revm_command_add(CMD_APPEND  , (void *) cmd_append  , (void *) revm_getoption2   , 1, HLP_APPEND);
  revm_command_add(CMD_EXTEND  , (void *) cmd_extend  , (void *) revm_getoption2   , 1, HLP_EXTEND);
  revm_command_add(CMD_RELINJCT, (void *) cmd_relinject,(void *) revm_getoption2   , 0, HLP_RELINJCT);
  revm_command_add(CMD_HIJACK  , (void *) cmd_hijack  , (void *) revm_getvarparams , 1, HLP_HIJACK);
  revm_command_add(CMD_CMP     , (void *) cmd_cmp     , (void *) revm_getoption2   , 0, HLP_CMP);
  revm_command_add(CMD_CMP2    , (void *) cmd_cmp     , (void *) revm_getoption2   , 0, HLP_CMP);
  revm_command_add(CMD_TEST    , (void *) cmd_test    , (void *) revm_getoption2   , 0, HLP_TEST);

  revm_command_add(CMD_INSERT  , (void *) cmd_insert  , (void *) revm_getvarparams , 1, HLP_INSERT);
  revm_command_add(CMD_INSERT2 , (void *) cmd_insert  , (void *) revm_getvarparams , 1, HLP_INSERT);
  revm_command_add(CMD_REMOVE  , (void *) cmd_remove  , (void *) revm_getoption2   , 1, HLP_REMOVE);
  revm_command_add(CMD_REMOVE2 , (void *) cmd_remove  , (void *) revm_getoption2   , 1, HLP_REMOVE);
  revm_command_add(CMD_FLUSH   , (void *) cmd_flush   , (void *) NULL            , 1, HLP_FLUSH);
  revm_command_add(CMD_COLOR   , (void *) cmd_color   , (void *) revm_getoption3   , 0, HLP_COLOR);
  revm_command_add(CMD_NOCOLOR , (void *) cmd_nocolor , (void *) NULL            , 0, HLP_NOCOLOR);

  revm_command_add(CMD_JMP     , (void *) cmd_jmp     , (void *) revm_getoption    , 0, HLP_JMP);
  revm_command_add(CMD_JE      , (void *) cmd_je      , (void *) revm_getoption    , 0, HLP_JE);
  revm_command_add(CMD_JNE     , (void *) cmd_jne     , (void *) revm_getoption    , 0, HLP_JNE);
  revm_command_add(CMD_JG      , (void *) cmd_jg      , (void *) revm_getoption    , 0, HLP_JG);
  revm_command_add(CMD_JL      , (void *) cmd_jl      , (void *) revm_getoption    , 0, HLP_JL);
  revm_command_add(CMD_JGE     , (void *) cmd_jge     , (void *) revm_getoption    , 0, HLP_JGE);
  revm_command_add(CMD_JLE     , (void *) cmd_jle     , (void *) revm_getoption    , 0, HLP_JLE);

  revm_command_add(CMD_FOREACH , (void *) cmd_foreach , (void *) revm_getforparams , 0, HLP_FOREACH);
  revm_command_add(CMD_FOREND  , (void *) cmd_forend  , (void *) NULL              , 0, HLP_FOREND);
  revm_command_add(CMD_MATCH   , (void *) cmd_match   , (void *) revm_getmatchparams, 0, HLP_MATCH);
  revm_command_add(CMD_MATCHEND, (void *) cmd_matchend, (void *) NULL              , 0, HLP_MATCHEND);
  revm_command_add(CMD_CASE    , (void *) cmd_case    , (void *) revm_getvarparams , 0, HLP_CASE);
  revm_command_add(CMD_DEFAULT , (void *) cmd_default , (void *) revm_getvarparams , 0, HLP_DEFAULT);
  revm_command_add(CMD_REFLECT , (void *) cmd_reflect , (void *) revm_getoption    , 0, HLP_REFLECT);

  revm_command_add(CMD_SDIR    , (void *) cmd_scriptsdir, (void *) revm_getoption  , 0, HLP_SDIR);
  revm_command_add(CMD_VLIST   , (void *) cmd_vlist   , (void *) NULL            , 0, HLP_VLIST);
  revm_command_add(CMD_SOURCE  , (void *) cmd_source  , (void *) revm_getvarparams , 0, HLP_SOURCE);
  revm_command_add(CMD_LSCRIPTS, (void *) cmd_lscripts, (void *) NULL		  , 0, HLP_LSCRIPTS);
  revm_command_add(CMD_CAT	, (void *) cmd_cat     , (void *) revm_getoption    , 0, HLP_CAT);
  revm_command_add(CMD_PROFILE	, (void *) cmd_profile , (void *) revm_getvarparams , 0, HLP_PROFILE);
  revm_command_add(CMD_LOG     , (void *) cmd_log     , (void *) revm_getvarparams , 0, HLP_LOG);
  revm_command_add(CMD_EXPORT  , (void *) cmd_export  , (void *) revm_getoption2   , 0, HLP_EXPORT);
  revm_command_add(CMD_SHARED  , (void *) cmd_shared  , (void *) NULL            , 0, HLP_SHARED);
  revm_command_add(CMD_VERSION , (void *) cmd_version , (void *) revm_getregxoption, 1, HLP_VERSION);
  revm_command_add(CMD_VERNEED , (void *) cmd_verneed , (void *) revm_getregxoption, 1, HLP_VERNEED);
  revm_command_add(CMD_VERDEF  , (void *) cmd_verdef  , (void *) revm_getregxoption, 1, HLP_VERDEF);
  revm_command_add(CMD_HASH    , (void *) cmd_hashx   , (void *) revm_getregxoption, 1, HLP_HASH);
  revm_command_add(CMD_CONFIGURE, cmd_configure       , revm_getvarparams, 0, HLP_CONFIGURE);

  /* Type related commands */
  revm_command_add(CMD_TYPE     , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
  revm_command_add(CMD_TYPEDEF  , cmd_typedef         , revm_getvarparams, 0, HLP_TYPEDEF);
  revm_command_add(CMD_INFORM   , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM , cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);
  revm_command_add(CMD_INFORM2  , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM2, cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);
  
#if defined(ELFSHNET)
  /* DUMP network commands */
  revm_command_add(CMD_NETWORK   , (void *) cmd_network  , (void *) NULL            , 0, HLP_NETWORK);
  revm_command_add(CMD_NETWORK2  , (void *) cmd_network  , (void *) NULL            , 0, HLP_NETWORK);
  revm_command_add(CMD_NETLIST   , (void *) cmd_netlist  , (void *) NULL            , 0, HLP_NETLIST);
  revm_command_add(CMD_NETKILL   , (void *) cmd_netkill  , (void *) revm_getoption    , 0, HLP_NETKILL);
  revm_command_add(CMD_PEERSLIST , (void *) cmd_peerslist, (void *) NULL            , 0, HLP_PEERSLIST);
  revm_command_add(CMD_CONNECT   , (void *) cmd_connect  , (void *) revm_getoption    , 0, HLP_CONNECT);
  revm_command_add(CMD_DISCON    , (void *) cmd_discon   , (void *) revm_getoption    , 0, HLP_DISCON);
  revm_command_add(CMD_RCMD	  , (void *) cmd_rcmd     , (void *) revm_getvarparams , 0, HLP_RCMD);
#endif

  /* Flow analysis commands */
  revm_command_add(CMD_ANALYSE	  , cmd_analyse       , NULL,            1, HLP_ANALYSE);
  revm_command_add(CMD_UNSTRIP	  , cmd_unstrip       , NULL,            1, HLP_UNSTRIP);
  revm_command_add(CMD_GRAPH     , cmd_graph         , revm_getvarparams, 1, HLP_GRAPH);
  revm_command_add(CMD_INSPECT   , cmd_inspect       , revm_getoption,    1, HLP_INSPECT);
  revm_command_add(CMD_FLOWJACK  , cmd_flowjack      , revm_getoption2,   1, HLP_FLOWJACK);
  revm_command_add(CMD_ADDGOTO   , cmd_addgoto       , revm_getoption2,   1, HLP_ADDGOTO);
  revm_command_add(CMD_SETGVL    , cmd_setgvl        , revm_getoption,    1, HLP_SETGVL);
  revm_command_add(CMD_RENAME	  , cmd_rename        , revm_getoption2,   1, HLP_RENAME);  
  revm_command_add(CMD_CONTROL   , cmd_control       , NULL,            1, HLP_CONTROL);

  /* Base type declarations commands */
  typeinfo = aspect_basetype_get(&typenbr);
  for (index = 0; index < typenbr; index++)
    revm_command_add(typeinfo[index].name, cmd_declare, 
	      revm_getvarparams, 0, HLP_DECLARE);
}

/**
 * Mix default library path with LD_LIBRARY_PATH variable 
 */
static char	*get_libpath()
{
  int		len;
  char		*ldenv;

  /* Set default value */
  strncpy(elfsh_libpath, ELFSH_LIBPATH, BUFSIZ);
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
  revmobj_t	*g;
  revmobj_t	*r;
  revmobj_t	*s;
  revmobj_t	*e;
  revmobj_t	*l;
  revmobj_t	*o;

  f = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0);
  g = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0);
  r = revm_create_IMMED(ASPECT_TYPE_INT, 1, 0xFFFFFFFF);
  s = revm_create_IMMEDSTR(1, ELFSH_SHELL);
  e = revm_create_IMMEDSTR(1, ELFSH_EDITOR);
  l = revm_create_IMMEDSTR(1, get_libpath());
  o = revm_create_IMMED(ASPECT_TYPE_INT, 1, ELFSH_SLOG);

  hash_init(&vars_hash, "variables", 251, ASPECT_TYPE_UNKNOW);
  hash_add(&vars_hash, REVM_VAR_RESULT, f);
  hash_add(&vars_hash, REVM_VAR_LOAD, g);
  hash_add(&vars_hash, REVM_VAR_ERROR, r);
  hash_add(&vars_hash, REVM_VAR_SHELL, s);
  hash_add(&vars_hash, REVM_VAR_EDITOR, e);
  hash_add(&vars_hash, REVM_VAR_LIBPATH, l);
  hash_add(&vars_hash, REVM_VAR_STRIPLOG, o);
}


/**
 * Setup ELF constants hash tables 
 */
static void	setup_consthash()
{
  u_int		index;

  hash_init(&const_hash, "constants", 51, ASPECT_TYPE_STR);

  for (index = 0; index < ELFSH_SEGTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_seg_type[index].name, 
	     elfsh_seg_type + index);

  for (index = 0; index < ELFSH_EXTSEG_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_extseg_type[index].name, 
	     elfsh_extseg_type + index);

  for (index = 0; index < ELFSH_ENCODING_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_encoding[index].name, 
	     elfsh_encoding + index);
  for (index = 0; index < ELFSH_SHTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_sh_type[index].name, 
	     elfsh_sh_type + index);
  for (index = 0; index < ELFSH_OBJTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_obj_type[index].name, 
	     elfsh_obj_type + index);
  for (index = 0; index < ELFSH_SYMBIND_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_sym_bind[index].name, 
	     elfsh_sym_bind + index);
  for (index = 0; index < ELFSH_SYMTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_sym_type[index].name, 
	     elfsh_sym_type + index);
  for (index = 0; index < ELFSH_RELOC_i386_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_i386[index].name, 
	     elfsh_rel_type_i386 + index);

  for (index = 0; index < ELFSH_RELOC_SPARC64_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_sparc[index].name, 
	     elfsh_rel_type_sparc + index);

  for (index = 0; index < ELFSH_RELOC_IA64_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_ia64[index].name, 
	     elfsh_rel_type_ia64 + index);

  for (index = 0; index < ELFSH_RELOC_ALPHA_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_alpha[index].name, 
	     elfsh_rel_type_alpha + index);

  for (index = 0; index < ELFSH_RELOC_MIPS_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_mips[index].name, 
	     elfsh_rel_type_mips + index);

     
  for (index = 0; index < ELFSH_DYNAMIC_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_dynentry_type[index].name, 
	     elfsh_dynentry_type + index);
  for (index = 0; index < ELFSH_EXTDYN_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_extdyn_type[index].name, 
	     elfsh_extdyn_type + index);
  for (index = 0; index < ELFSH_MIPSDYN_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_mipsdyn_type[index].name, 
	     elfsh_mipsdyn_type + index);
  for (index = 0; index < ELFSH_FEATURE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_feature1[index].name, 
	     elfsh_feature1 + index);
  for (index = 0; index < ELFSH_POSFLAG_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_posflag1[index].name, 
	     elfsh_posflag1 + index);
  for (index = 0; index < ELFSH_FLAGS_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_flags[index].name, 
	     elfsh_flags + index);
  for (index = 0; index < ELFSH_FLAGS1_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_flags1[index].name, 
	     elfsh_flags1 + index);
  for (index = 0; index < ELFSH_MIPSFLAGS_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_mipsflags[index].name, 
	     elfsh_mipsflags + index);
}


/** 
 * Setup default grammar
 * XXX: This function is temporary and we not remain in the new type system 
 */
void setup_grammar()
{
  hash_init(&parser_hash, "parsers", 11, ASPECT_TYPE_CADDR);

  /* Default grammar rules */
#define LOOKUP5_IDX "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]][%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_A   "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^:]:%41[^%%]%%%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_B   "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^:]:%41[^]]]"REVM_SEP"%41s"
#define LOOKUP4_C   "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"REVM_SEP"%41s"
#define LOOKUP3_IDX "%41[^"REVM_SEP"]"REVM_SEP"%41[^[][%41[^]]]"
#define LOOKUP3     "%41[^"REVM_SEP"]"REVM_SEP"%41[^"REVM_SEP"]"REVM_SEP"%41s"
#define LOOKUP_VECT "$vector[%41[^]]]"
#define LOOKUP_HASH "$hash[%41[^]]]"
#define LOOKUP_LIST "$list[%41[^]]]"

  hash_add(&parser_hash, LOOKUP5_IDX, parse_lookup5_index);
  hash_add(&parser_hash, LOOKUP4_A, parse_lookup4);
  hash_add(&parser_hash, LOOKUP4_B, parse_lookup4);
  hash_add(&parser_hash, LOOKUP4_C, parse_lookup4);
  hash_add(&parser_hash, LOOKUP3_IDX, parse_lookup3_index);
  hash_add(&parser_hash, LOOKUP3, parse_lookup3);
  hash_add(&parser_hash, LOOKUP_VECT, parse_vector);
  hash_add(&parser_hash, LOOKUP_HASH, parse_hash);
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
 * Setup all used sub functions 
 */
void setup_traces_table()
{
  traces_addcmd("add"      , (void *) traces_add      , 2, 1);
  traces_addcmd("rm"       , (void *) traces_rm       , 2, 1);
  traces_addcmd("exclude"  , (void *) traces_exclude  , 2, 1);
  traces_addcmd("rmexclude", (void *) traces_rmexclude, 2, 1);
  traces_addcmd("enable"   , (void *) traces_enable   , 2, 1);
  traces_addcmd("disable"  , (void *) traces_disable  , 2, 1);
  traces_addcmd("create"   , (void *) traces_create   , 2, 1);
  traces_addcmd("delete"   , (void *) traces_delete   , 2, 0);
  traces_addcmd("flush"    , (void *) traces_flush    , 2, 0);
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
  hash_init(&labels_hash[0]   , "labels"     , 51, ASPECT_TYPE_STR);
  hash_init(&fg_color_hash    , "fgcolors"   , 13, ASPECT_TYPE_UNKNOW);
  hash_init(&bg_color_hash    , "bgcolors"   , 13, ASPECT_TYPE_UNKNOW);
  hash_init(&t_color_hash     , "tcolors"    , 11, ASPECT_TYPE_UNKNOW);
  hash_init(&traces_cmd_hash  , "traces"     , 11, ASPECT_TYPE_UNKNOW);
  hash_init(&world.shared_hash, "sharedfiles", 11, ASPECT_TYPE_UNKNOW);
  hash_init(&exprs_hash       , "expressions", 51, ASPECT_TYPE_EXPR);
  hash_init(&exprtypes_hash   , "exprtypes"  , 51, ASPECT_TYPE_UNKNOW);
  hash_init(&exprtypes_hash   , "instrlists" , 51, ASPECT_TYPE_LIST);
  setup_varshash();
  setup_cmdhash();
  setup_consthash();
  setup_L1hash();
  setup_L2hash();
  setup_color();
  setup_color_type();
  setup_traces_table();
  setup_grammar();
  aspect_init();
}
