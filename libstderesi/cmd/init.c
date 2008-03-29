/*
** @file init.c for libstderesi in ERESI
** @ingroup libstderesi
** @brief Constructors for the ERESI standard library
**
** $Id: init.c,v 1.4 2007-12-09 23:00:18 may Exp $
*/
#include <libstderesi.h>


/** 
 * @brief Setup the command hash table 
 */
void		eresi_commands_init()
{
  typeinfo_t	*typeinfo;
  unsigned int	typenbr;
  unsigned int	index;

  hash_init(&cmd_hash, "commands", 101, ASPECT_TYPE_UNKNOW);

  /* Interactive mode / Scripting mode commands */
  if (world.state.revm_mode != REVM_STATE_CMDLINE)
    {
      revm_command_add(CMD_LOAD      , (void *) cmd_load     , (void *) revm_getoption , 0, HLP_LOAD);
      revm_command_add(CMD_UNLOAD    , (void *) cmd_unload   , (void *) revm_getoption , 0, HLP_UNLOAD);
      revm_command_add(CMD_SAVE      , (void *) cmd_save     , (void *) revm_getoption , 1, HLP_SAVE);
      revm_command_add(CMD_SWITCH    , (void *) cmd_doswitch , (void *) revm_getoption , 0, HLP_SWITCH);
      revm_command_add(CMD_METACMD   , (void *) cmd_meta     , (void *) NULL           , 0, HLP_METACMD);
      revm_command_add(CMD_QUIT      , (void *) cmd_quit     , (void *) NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_QUIT2     , (void *) cmd_quit     , (void *) NULL           , 0, HLP_QUIT);
      revm_command_add(CMD_RETURN    , (void *) cmd_return   , (void *) revm_getoption , 0, HLP_RETURN);
      revm_command_add(CMD_RETURN2   , (void *) cmd_return   , (void *) revm_getoption , 0, HLP_RETURN);
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
      revm_command_add(CMD_DEFINE    , (void *) cmd_define   , (void *) revm_getvarparams, 0, HLP_DEFINE);
      revm_command_add(CMD_UNDEF     , (void *) cmd_undef    , (void *) revm_getoption   , 0, HLP_UNDEF);
    }

  /* Command line only commands */
  else
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
  revm_command_add(CMD_MOD     , (void *) cmd_mod     , (void *) revm_getoption2   , 0, HLP_MOD);
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
  revm_command_add(CMD_CASE    , (void *) cmd_case    , (void *) revm_getcaseparams , 0, HLP_CASE);

  revm_command_add(CMD_PRE    , (void *) cmd_pre    , (void *) revm_getvarparams , 0, HLP_PRE);
  revm_command_add(CMD_POST    , (void *) cmd_post   , (void *) revm_getvarparams , 0, HLP_POST);
  revm_command_add(CMD_INTO    , (void *) cmd_into   , (void *) revm_getcaseparams , 0, HLP_INTO);

  revm_command_add(CMD_DEFAULT , (void *) cmd_default , (void *) revm_getvarparams , 0, HLP_DEFAULT);
  revm_command_add(CMD_REFLECT , (void *) cmd_reflect , (void *) revm_getoption    , 0, HLP_REFLECT);

  revm_command_add(CMD_SDIR    , (void *) cmd_scriptsdir, (void *) revm_getoption  , 0, HLP_SDIR);
  revm_command_add(CMD_VLIST   , (void *) cmd_vlist   , (void *) revm_getregxoption, 0, HLP_VLIST);
  revm_command_add(CMD_VARLIST , (void *) cmd_vlist   , (void *) revm_getregxoption, 0, HLP_VLIST);
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
  revm_command_add(CMD_UNION    , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
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
 * @brief Constructor for libstderesi
 */
void		eresi_stdconstructor()
{
  world.cmd_init = eresi_commands_init;
}
