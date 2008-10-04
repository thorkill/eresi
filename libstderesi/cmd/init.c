/*
** @file init.c for libstderesi in ERESI
** @ingroup libstderesi
** @brief Constructors for the ERESI standard library
**
** $Id: init.c,v 1.4 2007-12-09 23:00:18 may Exp $
*/
#include <libstderesi.h>

/* The command hash table : hash the command name and returns a revmcmd_t */
hash_t		cmd_hash;

/* Tracing subcommands hash */
hash_t		traces_cmd_hash;

/* The constants hash tables : ASCII shortcuts for ERESI */
hash_t		const_hash;

/**
 * Setup ELF constants hash tables 
 */
static void	eresi_constants_init()
{
  unsigned int		index;

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

  for (index = 0; index < ELFSH_INSTRTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) asm_instr_type[index].name, 
	     asm_instr_type + index);

  for (index = 0; index < ELFSH_OPTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) asm_operand_type[index].name, 
	     asm_operand_type + index);

  for (index = 0; index < ELFSH_LINKTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_link_type[index].name, 
	     elfsh_link_type + index);

  for (index = 0; index < ELFSH_LINKSCOPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_link_scope[index].name, 
	     elfsh_link_scope + index);

  for (index = 0; index < ELFSH_ARCHTYPE_MAX; index++)
    hash_add(&const_hash, 
	     (char *) elfsh_arch_type[index].name, 
	     elfsh_arch_type + index);
}


/** 
 * @brief Setup the command hash table 
 */
void		eresi_commands_init()
{
  typeinfo_t	*typeinfo;
  unsigned int	typenbr;
  unsigned int	index;

  hash_init(&cmd_hash, "commands", 101, ASPECT_TYPE_UNKNOW);
  hash_init(&traces_cmd_hash  , "traces", 11, ASPECT_TYPE_UNKNOW);
  eresi_constants_init();

  /* Interactive mode / Scripting mode commands */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    {
      revm_command_add(CMD_LOAD      , cmd_load     , revm_getoption , 0, HLP_LOAD);
      revm_command_add(CMD_UNLOAD    , cmd_unload   , revm_getoption , 0, HLP_UNLOAD);
      revm_command_add(CMD_SAVE      , cmd_save     , revm_getoption , 1, HLP_SAVE);
      revm_command_add(CMD_SWITCH    , cmd_doswitch , revm_getoption , 0, HLP_SWITCH);
      revm_command_add(CMD_METACMD   , cmd_meta     , NULL           , 0, HLP_METACMD);
      revm_command_add(CMD_QUIT      , cmd_quit     , NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_QUIT2     , cmd_quit     , NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_RETURN    , cmd_return   , revm_getoption , 0, HLP_RETURN);
      revm_command_add(CMD_RETURN2   , cmd_return   , revm_getoption , 0, HLP_RETURN);
      revm_command_add(CMD_LIST      , cmd_dolist   , NULL           , 0, HLP_LIST);
      revm_command_add(CMD_LIST2     , cmd_dolist   , NULL           , 0, HLP_LIST);
      revm_command_add(CMD_STOP      , cmd_stop     , NULL           , 0, HLP_STOP);
      revm_command_add(CMD_WORKSPACE , cmd_workspace, revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_WORKSPACE2, cmd_workspace, revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_WORKSPACE2, cmd_workspace, revm_getvarparams, 0, HLP_WORKSPACE);
      revm_command_add(CMD_VECTORS   , cmd_vectors  , revm_getvarparams, 0, HLP_VECTORS);
      revm_command_add(CMD_TABLES    , cmd_tables   , revm_getvarparams, 0, HLP_TABLES);
      revm_command_add(CMD_LISTS     , cmd_lists    , revm_getvarparams, 0, HLP_LISTS);
      revm_command_add(CMD_EMPTY     , cmd_empty    , revm_getvarparams, 0, HLP_EMPTY);
      revm_command_add(CMD_DEFINE    , cmd_define   , revm_getvarparams, 0, HLP_DEFINE);
      revm_command_add(CMD_UNDEF     , cmd_undef    , revm_getoption   , 0, HLP_UNDEF);
    }

  /* Command line only commands */
  else
    {
      revm_command_add(CMD_BINFILE_W, NULL, revm_getoutput, 0, "");
      revm_command_add(CMD_BINFILE_R, NULL, revm_getinput , 0, "");
    }

  /* General purpose command */
  revm_command_add(CMD_MODLOAD , cmd_modload  , revm_getoption   , 0, HLP_MODLOAD);
  revm_command_add(CMD_MODULOAD, cmd_modunload, revm_getoption   , 0, HLP_MODULOAD);
  revm_command_add(CMD_DISASM  , cmd_disasm  , revm_getdisasm    , 1, HLP_DISASM);
  revm_command_add(CMD_DISASM2 , cmd_disasm  , revm_getdisasm    , 1, HLP_DISASM);
  revm_command_add(CMD_HEXA    , cmd_disasm  , revm_gethexa      , 1, HLP_HEXA);
  revm_command_add(CMD_HEXA2   , cmd_disasm  , revm_gethexa      , 1, HLP_HEXA);
  revm_command_add(CMD_PRINT   , cmd_print   , revm_getvarparams , 0, HLP_PRINT);
  revm_command_add(CMD_EXEC    , cmd_exec    , revm_getvarparams , 0, HLP_EXEC);
  revm_command_add(CMD_EDIT    , cmd_edit    , revm_getoption    , 0, HLP_EDIT);
  revm_command_add(CMD_PHT     , cmd_pht     , revm_getregxoption, 1, HLP_PHT);
  revm_command_add(CMD_GOT     , cmd_got     , revm_getregxoption, 1, HLP_GOT);
  revm_command_add(CMD_CTORS   , cmd_ctors   , revm_getregxoption, 1, HLP_CTORS);
  revm_command_add(CMD_DTORS   , cmd_dtors   , revm_getregxoption, 1, HLP_DTORS);
  revm_command_add(CMD_NOTE    , cmd_notes   , revm_getregxoption, 1, HLP_NOTES);
  revm_command_add(CMD_COREINFO, cmd_coreinfo, NULL	          , 1, HLP_COREINFO);
  revm_command_add(CMD_SYM     , cmd_sym     , revm_getregxoption, 1, HLP_SYM);
  revm_command_add(CMD_DYNAMIC , cmd_dyn     , revm_getregxoption, 1, HLP_DYNAMIC);
  revm_command_add(CMD_DYNSYM  , cmd_dynsym  , revm_getregxoption, 1, HLP_DYNSYM);
  revm_command_add(CMD_SHT     , cmd_sht     , revm_getregxoption, 1, HLP_SHT);
  revm_command_add(CMD_DEBUG   , cmd_debug   , revm_getvarparams , 0, HLP_DEBUG);
  revm_command_add(CMD_REL     , cmd_rel     , revm_getregxoption, 1, HLP_REL);
  revm_command_add(CMD_COMMENT , cmd_comments, revm_getregxoption, 1, HLP_COMMENT);
  revm_command_add(CMD_PHT2    , cmd_pht     , revm_getregxoption, 1, HLP_PHT);
  revm_command_add(CMD_GOT2    , cmd_got     , revm_getregxoption, 1, HLP_GOT);
  revm_command_add(CMD_CTORS2  , cmd_ctors   , revm_getregxoption, 1, HLP_CTORS);
  revm_command_add(CMD_DTORS2  , cmd_dtors   , revm_getregxoption, 1, HLP_DTORS);
  revm_command_add(CMD_NOTE2   , cmd_notes   , revm_getregxoption, 1, HLP_NOTES);
  revm_command_add(CMD_SYM2    , cmd_sym     , revm_getregxoption, 1, HLP_SYM);
  revm_command_add(CMD_DYNAMIC2, cmd_dyn     , revm_getregxoption, 1, HLP_DYNAMIC);
  revm_command_add(CMD_DYNSYM2 , cmd_dynsym  , revm_getregxoption, 1, HLP_DYNSYM);
  revm_command_add(CMD_SHT2    , cmd_sht     , revm_getregxoption, 1, HLP_SHT);
  revm_command_add(CMD_REL2    , cmd_rel     , revm_getregxoption, 1, HLP_REL);
  revm_command_add(CMD_COMMENT2, cmd_comments, revm_getregxoption, 1, HLP_COMMENT);
  revm_command_add(CMD_SET     , cmd_set     , revm_getoption2   , 0, HLP_SET);
  revm_command_add(CMD_ADD     , cmd_add     , revm_getoption2   , 0, HLP_ADD);
  revm_command_add(CMD_SUB     , cmd_sub     , revm_getoption2   , 0, HLP_SUB);
  revm_command_add(CMD_MUL     , cmd_mul     , revm_getoption2   , 0, HLP_MUL);
  revm_command_add(CMD_DIV     , cmd_div     , revm_getoption2   , 0, HLP_DIV);
  revm_command_add(CMD_MOD     , cmd_mod     , revm_getoption2   , 0, HLP_MOD);
  revm_command_add(CMD_INFO    , cmd_info    , NULL            , 0, HLP_INFO);
  revm_command_add(CMD_FIXUP   , cmd_fixup   , NULL            , 1, HLP_FIXUP);
  revm_command_add(CMD_ELF2    , cmd_elf     , NULL            , 1, HLP_ELF);
  revm_command_add(CMD_INTERP2 , cmd_interp  , NULL            , 1, HLP_INTERP);
  revm_command_add(CMD_ELF     , cmd_elf     , NULL            , 1, HLP_ELF);
  revm_command_add(CMD_INTERP  , cmd_interp  , NULL            , 1, HLP_INTERP);
  revm_command_add(CMD_HELP    , cmd_help    , revm_getvarparams , 0, HLP_HELP);
  revm_command_add(CMD_MODHELP , cmd_modhelp , revm_getoption    , 0, HLP_MODHELP);
  revm_command_add(CMD_QUIET   , cmd_quiet   , NULL            , 0, HLP_QUIET);
  revm_command_add(CMD_QUIET2  , cmd_quiet   , NULL            , 0, HLP_QUIET);
  revm_command_add(CMD_FORCE   , cmd_force   , NULL            , 0, HLP_FORCE);
  revm_command_add(CMD_VERB    , cmd_verb    , NULL            , 0, HLP_VERB);
  revm_command_add(CMD_VERB2   , cmd_verb    , NULL            , 0, HLP_VERB);
  revm_command_add(CMD_SORT    , cmd_sort    , revm_getoption    , 0, HLP_SORT);
  revm_command_add(CMD_SORT2   , cmd_sort    , revm_getoption    , 0, HLP_SORT);
  revm_command_add(CMD_ALL	, cmd_glregx  , revm_getoption    , 0, HLP_ALL);
  revm_command_add(CMD_ALL2	, cmd_glregx  , revm_getoption    , 0, HLP_ALL);
  revm_command_add(CMD_ALERT	, cmd_alert   , revm_getoption    , 0, HLP_ALERT);
  revm_command_add(CMD_FINDREL , cmd_findrel , NULL            , 1, HLP_FINDREL);
  revm_command_add(CMD_WRITE	, cmd_write   , revm_getvarparams , 1, HLP_WRITE);
  revm_command_add(CMD_APPEND  , cmd_append  , revm_getoption2   , 1, HLP_APPEND);
  revm_command_add(CMD_EXTEND  , cmd_extend  , revm_getoption2   , 1, HLP_EXTEND);
  revm_command_add(CMD_RELINJCT, cmd_relinject,(void *) revm_getoption2   , 0, HLP_RELINJCT);
  revm_command_add(CMD_HIJACK  , cmd_hijack  , revm_getvarparams , 1, HLP_HIJACK);
  revm_command_add(CMD_CMP     , cmd_cmp     , revm_getoption2   , 0, HLP_CMP);
  revm_command_add(CMD_CMP2    , cmd_cmp     , revm_getoption2   , 0, HLP_CMP);
  revm_command_add(CMD_TEST    , cmd_test    , revm_getoption2   , 0, HLP_TEST);

  revm_command_add(CMD_INSERT  , cmd_insert  , revm_getvarparams , 1, HLP_INSERT);
  revm_command_add(CMD_INSERT2 , cmd_insert  , revm_getvarparams , 1, HLP_INSERT);
  revm_command_add(CMD_REMOVE  , cmd_remove  , revm_getoption2   , 1, HLP_REMOVE);
  revm_command_add(CMD_REMOVE2 , cmd_remove  , revm_getoption2   , 1, HLP_REMOVE);
  revm_command_add(CMD_FLUSH   , cmd_flush   , NULL            , 1, HLP_FLUSH);
  revm_command_add(CMD_COLOR   , cmd_color   , revm_getoption3   , 0, HLP_COLOR);
  revm_command_add(CMD_NOCOLOR , cmd_nocolor , NULL            , 0, HLP_NOCOLOR);

  revm_command_add(CMD_JMP     , cmd_jmp     , revm_getoption    , 0, HLP_JMP);
  revm_command_add(CMD_JE      , cmd_je      , revm_getoption    , 0, HLP_JE);
  revm_command_add(CMD_JNE     , cmd_jne     , revm_getoption    , 0, HLP_JNE);
  revm_command_add(CMD_JG      , cmd_jg      , revm_getoption    , 0, HLP_JG);
  revm_command_add(CMD_JL      , cmd_jl      , revm_getoption    , 0, HLP_JL);
  revm_command_add(CMD_JGE     , cmd_jge     , revm_getoption    , 0, HLP_JGE);
  revm_command_add(CMD_JLE     , cmd_jle     , revm_getoption    , 0, HLP_JLE);

  revm_command_add(CMD_FOREACH , cmd_foreach , revm_getforparams , 0, HLP_FOREACH);
  revm_command_add(CMD_FOREND  , cmd_forend  , NULL              , 0, HLP_FOREND);
  revm_command_add(CMD_MATCH   , cmd_match   , revm_getmatchparams, 0, HLP_MATCH);
  revm_command_add(CMD_MATCHEND, cmd_matchend, NULL              , 0, HLP_MATCHEND);
  revm_command_add(CMD_CASE    , cmd_case    , revm_getcaseparams , 0, HLP_CASE);

  revm_command_add(CMD_PRE    , cmd_pre    , revm_getvarparams , 0, HLP_PRE);
  revm_command_add(CMD_POST    , cmd_post   , revm_getvarparams , 0, HLP_POST);
  revm_command_add(CMD_INTO    , cmd_into   , revm_getcaseparams , 0, HLP_INTO);

  revm_command_add(CMD_DEFAULT , cmd_default , revm_getvarparams , 0, HLP_DEFAULT);
  revm_command_add(CMD_REFLECT , cmd_reflect , revm_getoption    , 0, HLP_REFLECT);

  revm_command_add(CMD_SDIR    , cmd_scriptsdir, revm_getoption  , 0, HLP_SDIR);
  revm_command_add(CMD_VLIST   , cmd_vlist   , revm_getregxoption, 0, HLP_VLIST);
  revm_command_add(CMD_VARLIST , cmd_vlist   , revm_getregxoption, 0, HLP_VLIST);
  revm_command_add(CMD_SOURCE  , cmd_source  , revm_getvarparams , 0, HLP_SOURCE);
  revm_command_add(CMD_LSCRIPTS, cmd_lscripts, NULL		  , 0, HLP_LSCRIPTS);
  revm_command_add(CMD_CAT	, cmd_cat     , revm_getoption    , 0, HLP_CAT);
  revm_command_add(CMD_PROFILE	, cmd_profile , revm_getvarparams , 0, HLP_PROFILE);
  revm_command_add(CMD_LOG     , cmd_log     , revm_getvarparams , 0, HLP_LOG);
  revm_command_add(CMD_EXPORT  , cmd_export  , revm_getoption2   , 0, HLP_EXPORT);
  revm_command_add(CMD_SHARED  , cmd_shared  , NULL            , 0, HLP_SHARED);
  revm_command_add(CMD_VERSION , cmd_version , revm_getregxoption, 1, HLP_VERSION);
  revm_command_add(CMD_VERNEED , cmd_verneed , revm_getregxoption, 1, HLP_VERNEED);
  revm_command_add(CMD_VERDEF  , cmd_verdef  , revm_getregxoption, 1, HLP_VERDEF);
  revm_command_add(CMD_HASH    , cmd_hashx   , revm_getregxoption, 1, HLP_HASH);
  revm_command_add(CMD_CONFIGURE, cmd_configure, revm_getvarparams, 0, HLP_CONFIGURE);

  /* Type related commands */
  revm_command_add(CMD_TYPE     , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
  revm_command_add(CMD_UNION    , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
  revm_command_add(CMD_TYPEDEF  , cmd_typedef         , revm_getvarparams, 0, HLP_TYPEDEF);
  revm_command_add(CMD_INFORM   , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM , cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);
  revm_command_add(CMD_INFORM2  , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM2, cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);
  
#if defined(ERESI_NET)
  /* DUMP network commands */
  revm_command_add(CMD_NETWORK   , cmd_network  , NULL            , 0, HLP_NETWORK);
  revm_command_add(CMD_NETWORK2  , cmd_network  , NULL            , 0, HLP_NETWORK);
  revm_command_add(CMD_NETLIST   , cmd_netlist  , NULL            , 0, HLP_NETLIST);
  revm_command_add(CMD_NETKILL   , cmd_netkill  , revm_getoption    , 0, HLP_NETKILL);
  revm_command_add(CMD_PEERSLIST , cmd_peerslist, NULL            , 0, HLP_PEERSLIST);
  revm_command_add(CMD_CONNECT   , cmd_connect  , revm_getoption    , 0, HLP_CONNECT);
  revm_command_add(CMD_DISCON    , cmd_discon   , revm_getoption    , 0, HLP_DISCON);
  revm_command_add(CMD_RCMD	  , cmd_rcmd     , revm_getvarparams , 0, HLP_RCMD);
#endif
/*   revm_command_add(CMD_NETWORK_GDBSUPPORT, cmd_network_gdbsupport, revm_getvarparams, */
/* 		   0, HLP_NETWORKGDB); */
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

  /* Setup all used sub functions */
  traces_addcmd("add"      , traces_add      , 2, 1);
  traces_addcmd("rm"       , traces_rm       , 2, 1);
  traces_addcmd("exclude"  , traces_exclude  , 2, 1);
  traces_addcmd("rmexclude", traces_rmexclude, 2, 1);
  traces_addcmd("enable"   , traces_enable   , 2, 1);
  traces_addcmd("disable"  , traces_disable  , 2, 1);
  traces_addcmd("create"   , traces_create   , 2, 1);
  traces_addcmd("delete"   , traces_delete   , 2, 0);
  traces_addcmd("flush"    , traces_flush    , 2, 0);

  /* Base type declarations commands */
  typeinfo = aspect_basetype_get(&typenbr);
  for (index = 0; index < typenbr; index++)
    revm_command_add(typeinfo[index].name, cmd_declare, 
	      revm_getvarparams, 0, HLP_DECLARE);
}


/**
 * @brief Constructor for libstderesi
 */
void		eresi_stdconstructor()
{
  world.cmd_init = eresi_commands_init;
}
