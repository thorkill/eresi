/*
** tables.c for elfsh
**
** A C++ code for this may have been easier to implement
**
** Started on  Sat Jan 25 07:48:41 2003 mayhem
** Last update Fri Jul  4 03:21:25 2003 mayhem
*/
#include "elfsh.h"




/* The command hash table : hash the command name and returns a elfshcmd_t */
hash_t		cmd_hash;

/* The constants hash tables : ascii defines from elf.h and elfsh.h */
hash_t		const_hash;

/* The ELF shell module hash tables: returns a elfshmod_t */
hash_t		mod_hash;

/* The file hash table : hash the filename and returns a elfshobj_t pointer */
hash_t		file_hash;

/* The Level 1 object hash table : hash the object name and returns a elfshL1_t pointer */
hash_t		L1_hash;

/* The Level 2 object hash table list : hash the object name and returns a elfshL2_t pointer */
hash_t		elf_L2_hash;	/* For the ELF header fields */
hash_t		sht_L2_hash;	/* For the Sections/SHT objects */
hash_t		pht_L2_hash;	/* For the Program header table objects */
hash_t		sym_L2_hash;	/* For symbol object field */
hash_t		rel_L2_hash;	/* For relocation entry fields */
hash_t		dynsym_L2_hash;	/* For .dynsym objects */
hash_t		dyn_L2_hash;	/* For .dynamic objects */
hash_t		sct_L2_hash;	/* Section data (byte/word/dword/instr arrays) */

/* hash_t	got_L2_hash;	*/ /* GOT objects */




/* Fill all the Level 1 Objects hash tables */
static void	setup_L1hash()
{
  hash_init(&L1_hash, 29);
  hash_add(&L1_hash, "hdr"     , (void *) vm_create_L1ENT(elfsh_get_hdr,
							  NULL, NULL,
							  &elf_L2_hash,
							  NULL, NULL, NULL, 0));

  hash_add(&L1_hash, "pht"     , (void *) vm_create_L1ENT(elfsh_get_pht,
							  NULL, NULL,
							  &pht_L2_hash,
							  elfsh_get_pht_entry_by_index,
							  NULL, NULL, 
							  sizeof (Elf32_Phdr)));

  hash_add(&L1_hash, "symtab"  , (void *) vm_create_L1ENT(elfsh_get_symtab,
							  NULL,
							  elfsh_get_symbol_by_name,
							  &sym_L2_hash,
							  elfsh_get_symbol_by_index,
							  NULL, NULL,
							  sizeof (Elf32_Sym)));

  hash_add(&L1_hash, "dynsym"  , (void *) vm_create_L1ENT(elfsh_get_dynsymtab,
							  NULL,
							  elfsh_get_dynsymbol_by_name,
							  &dynsym_L2_hash,
							  elfsh_get_symbol_by_index,
							  NULL, NULL,
							  sizeof (Elf32_Sym)));


  hash_add(&L1_hash, "got"     , (void *) vm_create_L1ENT(elfsh_get_got,
							  NULL, 
							  elfsh_get_got_entry_by_name,
							  NULL,
							  elfsh_get_got_entry_by_index,
							  elfsh_get_got_entry,
							  elfsh_set_got_entry,
							  sizeof (long)));


  hash_add(&L1_hash, "dynamic" , (void *) vm_create_L1ENT(elfsh_get_dynamic,
							  NULL, NULL,
							  &dyn_L2_hash,
							  elfsh_get_dynamic_entry_by_index,
							  NULL, NULL,
							  sizeof (Elf32_Dyn)));

  hash_add(&L1_hash, "ctors"   , (void *) vm_create_L1ENT(elfsh_get_ctors,
							  NULL, 
							  elfsh_get_ctors_entry_by_name,
							  NULL,
							  elfsh_get_ctors_entry_by_index,
							  elfsh_get_ctors_entry,
							  elfsh_set_ctors_entry,
							  sizeof (long)));

  hash_add(&L1_hash, "dtors"   , (void *) vm_create_L1ENT(elfsh_get_dtors,
							  NULL, 
							  elfsh_get_dtors_entry_by_name,
							  NULL,
							  elfsh_get_dtors_entry_by_index,
							  elfsh_get_dtors_entry,
							  elfsh_set_dtors_entry,
							  sizeof (long)));
  
  hash_add(&L1_hash, "rel"     , (void *) vm_create_L1ENT(NULL,
							  elfsh_get_reloc,
							  elfsh_get_relent_by_name,
							  &rel_L2_hash,
							  elfsh_get_relent_by_index,
							  NULL, NULL,
							  sizeof (Elf32_Rel)));
  
  hash_add(&L1_hash, "sht"     , (void *) vm_create_L1ENT(elfsh_get_sht,
							  NULL,
							  elfsh_get_sht_entry_by_name,
							  &sht_L2_hash,
							  elfsh_get_sht_entry_by_index,
							  NULL, NULL,
							  sizeof (Elf32_Shdr)));
  
  hash_add(&L1_hash, "section", (void *) vm_create_L1ENT(elfsh_get_section_list, 
							 NULL,
							 elfsh_get_section_by_nam,
							 &sct_L2_hash,
							 elfsh_get_section_by_idx,
							 NULL, NULL,
							 sizeof (elfshsect_t)));
}


/* Hash table for ELF header fields */
static void	setup_elfhash()
{
  
  hash_init(&elf_L2_hash, 29);
  hash_add(&elf_L2_hash, "magic"        , vm_create_L2ENT(elfsh_get_magic, 
							  elfsh_set_magic, 0,
							  NULL, NULL, 
							  NULL, NULL));
							  
  hash_add(&elf_L2_hash, "class"        , vm_create_L2ENT(elfsh_get_class, 
							  elfsh_set_class, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "type"         , vm_create_L2ENT(elfsh_get_objtype, 
							  elfsh_set_objtype, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "machine"      , vm_create_L2ENT(elfsh_get_arch, 
							  elfsh_set_arch, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "version"      , vm_create_L2ENT(elfsh_get_version, 
							  elfsh_set_version, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "entry"        , vm_create_L2ENT(elfsh_get_entrypoint, 
							  elfsh_set_entrypoint, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phoff"        , vm_create_L2ENT(elfsh_get_phtoff, 
							  elfsh_set_phtoff, 0,
							  NULL, NULL,
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shoff"        , vm_create_L2ENT(elfsh_get_shtoff, 
							  elfsh_set_shtoff, 0,
							  NULL, NULL, NULL, NULL));
  hash_add(&elf_L2_hash, "flags"        , vm_create_L2ENT(elfsh_get_flags, 
							  elfsh_set_flags, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "ehsize"       , vm_create_L2ENT(elfsh_get_ehsize,
							  elfsh_set_ehsize, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phentsize"    , vm_create_L2ENT(elfsh_get_phentsize,
							  elfsh_set_phentsize, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shentsize"    , vm_create_L2ENT(elfsh_get_shentsize,
							  elfsh_set_shentsize, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "phnum"        , vm_create_L2ENT(elfsh_get_phtnbr,
							  elfsh_set_phtnbr, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shnum"        , vm_create_L2ENT(elfsh_get_shtnbr,
							  elfsh_set_shtnbr, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "shstrndx"     , vm_create_L2ENT(elfsh_get_shstrtab_index,
							  elfsh_set_shstrtab_index, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_pageexec" , vm_create_L2ENT(elfsh_get_pax_pageexec, 
							  elfsh_set_pax_pageexec, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_emultramp", vm_create_L2ENT(elfsh_get_pax_emultramp, 
							  elfsh_set_pax_emultramp, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_mprotect" , vm_create_L2ENT(elfsh_get_pax_mprotect, 
							  elfsh_set_pax_mprotect, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_randmmap" , vm_create_L2ENT(elfsh_get_pax_randmmap, 
							  elfsh_set_pax_randmmap, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_randexec" , vm_create_L2ENT(elfsh_get_pax_randexec, 
							  elfsh_set_pax_randexec, 0,
							  NULL, NULL, 
							  NULL, NULL));
  hash_add(&elf_L2_hash, "pax_segmexec" , vm_create_L2ENT(elfsh_get_pax_segmexec, 
							  elfsh_set_pax_segmexec, 0,
							  NULL, NULL, 
							  NULL, NULL));
}



/* Hash table for SHT fields */
static void	setup_shthash()
{
  hash_init(&sht_L2_hash, 29);
  hash_add(&sht_L2_hash, "type"     , vm_create_L2ENT(elfsh_get_section_type, 
						      elfsh_set_section_type, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "addr"     , vm_create_L2ENT(elfsh_get_section_addr, 
						      elfsh_set_section_addr, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "offset"   , vm_create_L2ENT(elfsh_get_section_foffset, 
						      elfsh_set_section_foffset, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "size"     , vm_create_L2ENT(elfsh_get_section_size, 
						      elfsh_set_section_size, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "link"     , vm_create_L2ENT(elfsh_get_section_link, 
						      elfsh_set_section_link, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "info"     , vm_create_L2ENT(elfsh_get_section_info, 
						      elfsh_set_section_info, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "align"    , vm_create_L2ENT(elfsh_get_section_align, 
						      elfsh_set_section_align, 
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "entsize"  , vm_create_L2ENT(elfsh_get_section_entsize, 
						      elfsh_set_section_entsize,
						      0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "a"  , vm_create_L2ENT(elfsh_get_section_allocflag, 
						elfsh_set_section_allocflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "w"  , vm_create_L2ENT(elfsh_get_section_writableflag, 
						elfsh_set_section_writableflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "x"  , vm_create_L2ENT(elfsh_get_section_execflag, 
						elfsh_set_section_execflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "s"  , vm_create_L2ENT(elfsh_get_section_strflag, 
						elfsh_set_section_strflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "m"  , vm_create_L2ENT(elfsh_get_section_mergeableflag, 
						elfsh_set_section_mergeableflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "l"  , vm_create_L2ENT(elfsh_get_section_linkflag, 
						elfsh_set_section_linkflag, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&sht_L2_hash, "o"  , vm_create_L2ENT(elfsh_get_section_orderflag, 
						elfsh_set_section_orderflag, 
						0, NULL, NULL, NULL, NULL));


}

/* Hash table for PHT fields */
static void	setup_phthash()
{
  hash_init(&pht_L2_hash, 29);
  hash_add(&pht_L2_hash, "type"  , vm_create_L2ENT(elfsh_get_segment_type, 
						   elfsh_set_segment_type, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "offset", vm_create_L2ENT(elfsh_get_segment_offset, 
						   elfsh_set_segment_offset, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "vaddr" , vm_create_L2ENT(elfsh_get_segment_vaddr, 
						   elfsh_set_segment_vaddr, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "paddr" , vm_create_L2ENT(elfsh_get_segment_paddr, 
						   elfsh_set_segment_paddr, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "filesz", vm_create_L2ENT(elfsh_get_segment_filesz, 
						   elfsh_set_segment_filesz, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "memsz" , vm_create_L2ENT(elfsh_get_segment_memsz, 
						   elfsh_set_segment_memsz, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "flags" , vm_create_L2ENT(elfsh_get_segment_flags, 
						   elfsh_set_segment_flags, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&pht_L2_hash, "align" , vm_create_L2ENT(elfsh_get_segment_align, 
						   elfsh_set_segment_align, 
						   0, NULL, NULL, NULL, NULL));
}


/* Hash table for symbol table */
static void	setup_symhash()
{
  hash_init(&sym_L2_hash, 23);

  hash_add(&sym_L2_hash, "name" , vm_create_L2ENT(NULL, NULL, ELFSH_OBJSTR, 
						  elfsh_get_symbol_name,
						  elfsh_set_symbol_name,
						  NULL, NULL));

  hash_add(&sym_L2_hash, "value", vm_create_L2ENT(elfsh_get_symbol_value, 
						  elfsh_set_symbol_value, 
						  0, NULL, NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "size" , vm_create_L2ENT(elfsh_get_symbol_size, 
						  elfsh_set_symbol_size, 
						  0, NULL, NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "bind" , vm_create_L2ENT(elfsh_get_symbol_bind, 
						  elfsh_set_symbol_bind, 
						  0, NULL, NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "type" , vm_create_L2ENT(elfsh_get_symbol_type, 
						  elfsh_set_symbol_type, 
						  0, NULL, NULL, NULL, NULL));
  hash_add(&sym_L2_hash, "other", vm_create_L2ENT(elfsh_get_symbol_visibility, 
						  elfsh_set_symbol_visibility, 
						  0, NULL, NULL, NULL, NULL));

  hash_add(&sym_L2_hash, "link", vm_create_L2ENT(elfsh_get_symbol_link, 
						 elfsh_set_symbol_link, 
						 0, NULL, NULL, NULL, NULL));


}

/* Hash table for dynamic symbol table */
static void	setup_dynsymhash()
{
  hash_init(&dynsym_L2_hash, 23);

  hash_add(&dynsym_L2_hash, "name" , vm_create_L2ENT(NULL, NULL, ELFSH_OBJSTR,
						     elfsh_get_dynsymbol_name,
						     elfsh_set_dynsymbol_name,
						     NULL, NULL));

  hash_add(&dynsym_L2_hash, "value", vm_create_L2ENT(elfsh_get_symbol_value, 
						     elfsh_set_symbol_value, 
						     0, NULL, NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "size" , vm_create_L2ENT(elfsh_get_symbol_size, 
						     elfsh_set_symbol_size, 
						     0, NULL, NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "bind" , vm_create_L2ENT(elfsh_get_symbol_bind, 
						     elfsh_set_symbol_bind, 
						     0, NULL, NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "type" , vm_create_L2ENT(elfsh_get_symbol_type, 
						     elfsh_set_symbol_type, 
						     0, NULL, NULL, NULL, NULL));
  hash_add(&dynsym_L2_hash, "other", vm_create_L2ENT(elfsh_get_symbol_visibility, 
						     elfsh_set_symbol_visibility, 
						     0, NULL, NULL, NULL, NULL));

  hash_add(&dynsym_L2_hash, "link", vm_create_L2ENT(elfsh_get_symbol_link, 
						    elfsh_set_symbol_link, 
						    0, NULL, NULL, NULL, NULL));
  

}

/* Hash table for .rel sections */
static void	setup_relhash()
{
  hash_init(&rel_L2_hash, 23);
  hash_add(&rel_L2_hash, "type"  , vm_create_L2ENT(elfsh_get_reltype, 
						   elfsh_set_reltype, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "sym"   , vm_create_L2ENT(elfsh_get_relsym, 
						   elfsh_set_relsym, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "offset", vm_create_L2ENT(elfsh_get_reloffset, 
						   elfsh_set_reloffset, 
						   0, NULL, NULL, NULL, NULL));
  hash_add(&rel_L2_hash, "addend", vm_create_L2ENT(elfsh_get_reladdend, 
						   elfsh_set_reladdend,
						   0, NULL, NULL, NULL, NULL));
}

/* Hash table for .dynamic section */
static void	setup_dynhash()
{
  hash_init(&dyn_L2_hash, 11);
  hash_add(&dyn_L2_hash, "val", vm_create_L2ENT(elfsh_get_dynentry_val, 
						elfsh_set_dynentry_val, 
						0, NULL, NULL, NULL, NULL));
  hash_add(&dyn_L2_hash, "tag", vm_create_L2ENT(elfsh_get_dynentry_type, 
						elfsh_set_dynentry_type, 
						0, NULL, NULL, NULL, NULL));
}



/* Hash tables for GOT L2 objects */
/*
static void	setup_gothash()
{
  hash_init(&got_L2_hash, 11);
  hash_add(&got_L2_hash, "val" , vm_create_L2ENT(elfsh_get_got));
  hash_add(&got_L2_hash, "idx" , vm_create_L2ENT());
  hash_add(&got_L2_hash, "addr", vm_create_L2ENT());
}
*/


/* Hash tables for sections data */
static void	setup_scthash()
{
  hash_init(&sct_L2_hash, 11);
  hash_add(&sct_L2_hash, "name", vm_create_L2ENT(NULL, NULL, ELFSH_OBJSTR,
						 elfsh_get_section_name,
						 elfsh_set_section_name,
						 NULL, NULL));
  
  hash_add(&sct_L2_hash, "raw", vm_create_L2ENT(NULL, NULL, 
						ELFSH_OBJRAW,
						NULL, NULL, 
						elfsh_get_section_data,
						elfsh_write_section_data));
}



/* Now comes Level 2 objects hash functions */
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
  /* setup_gothash(); */
}



/* Setup the command hash table */
static void	setup_cmdhash()
{
  hash_init(&cmd_hash, 101);

  /* Interactive mode only commands */
  if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
    {
      vm_addcmd(CMD_LOAD    , (void *) cmd_load     , (void *) vm_getoption, 0);
      vm_addcmd(CMD_UNLOAD  , (void *) cmd_unload   , (void *) vm_getoption, 0);
      vm_addcmd(CMD_SAVE    , (void *) cmd_save     , (void *) vm_getoption, 1);
      vm_addcmd(CMD_SWITCH  , (void *) cmd_doswitch , (void *) vm_getoption, 1);
      vm_addcmd(CMD_METACMD , (void *) cmd_meta     , (void *) NULL, 0);
      vm_addcmd(CMD_QUIT    , (void *) cmd_quit     , (void *) NULL, 0);
      vm_addcmd(CMD_QUIT2   , (void *) cmd_quit     , (void *) NULL, 0);
      vm_addcmd(CMD_LIST    , (void *) cmd_dolist   , (void *) NULL, 0);
      vm_addcmd(CMD_LIST2   , (void *) cmd_dolist   , (void *) NULL, 0);
      vm_addcmd(CMD_STOP    , (void *) cmd_stop     , (void *) NULL, 0);
    }
  
  /* Command line only commands */
  else
    {
      vm_addcmd(CMD_BINFILE_W, (void *) NULL, (void *) vm_getoutput, 0);
      vm_addcmd(CMD_BINFILE_R, (void *) NULL, (void *) vm_getinput , 0);
    }
  
  /* General purpose command */
  vm_addcmd(CMD_MODLOAD , (void *) cmd_modload  , (void *) vm_getoption, 0);
  vm_addcmd(CMD_MODULOAD, (void *) cmd_modunload, (void *) vm_getoption, 0);
  vm_addcmd(CMD_DISASM  , (void *) cmd_disasm  , (void *) vm_getdisasm    , 1);
  vm_addcmd(CMD_HEXA    , (void *) cmd_disasm  , (void *) vm_gethexa      , 1);
  vm_addcmd(CMD_DISASM2 , (void *) cmd_disasm  , (void *) vm_getdisasm    , 1);
  vm_addcmd(CMD_HEXA2   , (void *) cmd_disasm  , (void *) vm_gethexa      , 1);
  vm_addcmd(CMD_GET     , (void *) cmd_get     , (void *) vm_getoption    , 1);
  vm_addcmd(CMD_PRINT   , (void *) cmd_print   , (void *) vm_getvarparams , 1);
  vm_addcmd(CMD_EXEC    , (void *) cmd_exec    , (void *) vm_getvarparams , 0);
  vm_addcmd(CMD_PHT     , (void *) cmd_pht     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_GOT     , (void *) cmd_got     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_CTORS   , (void *) cmd_ctors   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DTORS   , (void *) cmd_dtors   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_NOTE    , (void *) cmd_notes   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_SYM     , (void *) cmd_sym     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DYNAMIC , (void *) cmd_dyn     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DYNSYM  , (void *) cmd_dynsym  , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_SHT     , (void *) cmd_sht     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_STAB    , (void *) cmd_stab    , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_REL     , (void *) cmd_rel     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_COMMENT , (void *) cmd_comments, (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_PHT2    , (void *) cmd_pht     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_GOT2    , (void *) cmd_got     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_CTORS2  , (void *) cmd_ctors   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DTORS2  , (void *) cmd_dtors   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_NOTE2   , (void *) cmd_notes   , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_SYM2    , (void *) cmd_sym     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DYNAMIC2, (void *) cmd_dyn     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_DYNSYM2 , (void *) cmd_dynsym  , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_SHT2    , (void *) cmd_sht     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_STAB2   , (void *) cmd_stab    , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_REL2    , (void *) cmd_rel     , (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_COMMENT2, (void *) cmd_comments, (void *) vm_getregxoption, 1);
  vm_addcmd(CMD_SET     , (void *) cmd_set     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_ADD     , (void *) cmd_add     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_SUB     , (void *) cmd_sub     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_MUL     , (void *) cmd_mul     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_DIV     , (void *) cmd_div     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_MOD     , (void *) cmd_set     , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_INFO    , (void *) cmd_info    , (void *) NULL            , 0);
  vm_addcmd(CMD_FIXUP   , (void *) cmd_fixup   , (void *) NULL            , 1);
  vm_addcmd(CMD_ELF2    , (void *) cmd_elf     , (void *) NULL            , 1);
  vm_addcmd(CMD_INTERP2 , (void *) cmd_interp  , (void *) NULL            , 1);
  vm_addcmd(CMD_ELF     , (void *) cmd_elf     , (void *) NULL            , 1);
  vm_addcmd(CMD_INTERP  , (void *) cmd_interp  , (void *) NULL            , 1);
  vm_addcmd(CMD_HELP    , (void *) cmd_help    , (void *) NULL            , 0);
  vm_addcmd(CMD_SHTRM   , (void *) cmd_shtrm   , (void *) NULL            , 1);
  vm_addcmd(CMD_QUIET   , (void *) cmd_quiet   , (void *) NULL            , 0);
  vm_addcmd(CMD_QUIET2  , (void *) cmd_quiet   , (void *) NULL            , 0);
  vm_addcmd(CMD_VERB    , (void *) cmd_verb    , (void *) NULL            , 0);
  vm_addcmd(CMD_VERB2   , (void *) cmd_verb    , (void *) NULL            , 0);
  vm_addcmd(CMD_SORT    , (void *) NULL        , (void *) vm_getsort      , 0);
  vm_addcmd(CMD_SORT2   , (void *) NULL        , (void *) vm_getsort      , 0);
  vm_addcmd(CMD_ALL	, (void *) NULL        , (void *) vm_getglregx    , 0);
  vm_addcmd(CMD_ALL2	, (void *) NULL        , (void *) vm_getglregx    , 0);
  vm_addcmd(CMD_FINDREL , (void *) cmd_findrel , (void *) NULL            , 1);
  vm_addcmd(CMD_STRIP   , (void *) cmd_strip   , (void *) NULL            , 1);
  vm_addcmd(CMD_SSTRIP  , (void *) cmd_sstrip  , (void *) NULL            , 1);
  vm_addcmd(CMD_WRITE	, (void *) cmd_write   , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_APPEND  , (void *) cmd_append  , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_EXTEND  , (void *) cmd_extend  , (void *) vm_getoption2   , 1);
  vm_addcmd(CMD_RELINJCT, (void *) cmd_relinject,(void *) vm_getoption2   , 0);
  vm_addcmd(CMD_HIJACK  , (void *) cmd_hijack  , (void *) vm_getoption2   , 1);
}



/* Setup ELF constants hash tables */
static void	setup_consthash()
{
  u_int		index;

  hash_init(&const_hash, 51);
  for (index = 0; index < ELFSH_SEGTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_seg_type[index].name, 
	     elfsh_seg_type + index);
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
     for (index = 0; index < ELFSH_RELOC_SPARC_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_rel_type_sparc[index].name, 
	     elfsh_rel_type_sparc + index);

  for (index = 0; index < ELFSH_DYNAMIC_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_dynentry_type[index].name, 
	     elfsh_dynentry_type + index);
  for (index = 0; index < ELFSH_EXTDYN_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_extdyn_type[index].name, 
	     elfsh_extdyn_type + index);
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
}

   
/* Setup all hash tables */
void		vm_setup_hashtables()
{
  hash_init(&file_hash, 251);
  hash_init(&mod_hash, 251);
  setup_cmdhash();
  setup_consthash();
  setup_L1hash();
  setup_L2hash();
}
