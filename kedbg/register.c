#include "kedbg.h"

void     kedbg_register_command(void)
{
  revm_command_add(CMD_START,     cmd_kedbgcont, NULL, 0, HLP_START);
  revm_command_add(CMD_CONTINUE,  cmd_kedbgcont, NULL, 0, HLP_CONTINUE);
  revm_command_add(CMD_CONTINUE2, cmd_kedbgcont, NULL, 0, HLP_CONTINUE);

  /* Debugger only script commands */
  revm_command_add(CMD_MODE, cmd_mode, revm_getvarparams, 0, HLP_MODE);
  revm_command_add(CMD_BP, cmd_bp, revm_getvarparams, 0, HLP_BP);
  revm_command_add(CMD_BP2, cmd_bp, revm_getvarparams, 0, HLP_BP);
  revm_command_add(CMD_DELETE, cmd_delete, revm_getvarparams, 0, HLP_DELETE);
  revm_command_add(CMD_LINKMAP, cmd_linkmap, revm_getvarparams, 1, HLP_LINKMAP);
  revm_command_add(CMD_GOT, cmd_got, revm_getvarparams, 1, HLP_GOT);
  revm_command_add(CMD_DUMPREGS, cmd_kedbg_dump_regs, revm_getvarparams, 0, HLP_DUMPREGS);
  revm_command_add(CMD_STEP, cmd_kedbgstep, revm_getvarparams, 0, HLP_STEP);
  revm_command_add(CMD_BT, cmd_bt, NULL, 0, HLP_BT);
  revm_command_add(CMD_BT2, cmd_bt, NULL, 0, HLP_BT);
  revm_command_add(CMD_DISPLAY, cmd_display, revm_getvarparams, 0, HLP_DISPLAY);
  revm_command_add(CMD_UNDISPLAY, cmd_undisplay, revm_getvarparams, 0, HLP_UNDISPLAY);
  revm_command_add(CMD_RSHT, cmd_rsht, revm_getregxoption, 1, HLP_RSHT);
  revm_command_add(CMD_RPHT, cmd_rpht, revm_getregxoption, 1, HLP_RPHT);
  revm_command_add(CMD_QUIT, cmd_kedbgquit, NULL, 0, HLP_QUIT);
  revm_command_add(CMD_QUIT2, cmd_kedbgquit, NULL, 0, HLP_QUIT);
  revm_command_add(CMD_DISASM, cmd_kedbgdisasm, revm_getdisasm, 0, HLP_DISASM);
  revm_command_add(CMD_DISASM2, cmd_kedbgdisasm, revm_getdisasm, 0, HLP_DISASM);
  revm_command_add(CMD_HEXA, cmd_kedbgdisasm, revm_gethexa, 1, HLP_HEXA);
  revm_command_add(CMD_HEXA2, cmd_kedbgdisasm, revm_gethexa, 1, HLP_HEXA);
  revm_command_add(CMD_IVT, cmd_kedbgprintivt, NULL, 0, HLP_IVT);
  revm_command_add(CMD_HOOKIVT, cmd_kedbghookivt, NULL, 0, HLP_HOOKIVT);
  revm_command_add(CMD_WRITE, cmd_write, revm_getvarparams, 1, HLP_WRITE);
  revm_command_add(CMD_PROC, cmd_kedbgproc , NULL, 0, HLP_PROC);
  revm_command_add(CMD_GRAPH, cmd_kedbggraph, revm_getvarparams, 1, HLP_GRAPH);

  revm_command_add(CMD_MONITOR, cmd_kedbgmonitor, revm_getvarparams, 1, HLP_MONITOR);

  /* Type related commands */
  revm_command_add(CMD_TYPE     , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
  revm_command_add(CMD_UNION    , cmd_type            , revm_getvarparams, 0, HLP_TYPE);
  revm_command_add(CMD_TYPEDEF  , cmd_typedef         , revm_getvarparams, 0, HLP_TYPEDEF);
  revm_command_add(CMD_INFORM   , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM , cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);
  revm_command_add(CMD_INFORM2  , cmd_inform          , revm_getvarparams, 1, HLP_INFORM);
  revm_command_add(CMD_UNINFORM2, cmd_uninform        , revm_getvarparams, 0, HLP_UNINFORM);

  /* Flow analysis commands */
  revm_command_add(CMD_ANALYSE    , cmd_analyse       , revm_getvarparams, 1, HLP_ANALYSE);
  revm_command_add(CMD_UNSTRIP    , cmd_unstrip       , NULL,            1, HLP_UNSTRIP);
  revm_command_add(CMD_GRAPH     , cmd_graph         , revm_getvarparams, 1, HLP_GRAPH);
  revm_command_add(CMD_INSPECT   , cmd_inspect       , revm_getoption,    1, HLP_INSPECT);
  revm_command_add(CMD_FLOWJACK  , cmd_flowjack      , revm_getoption2,   1, HLP_FLOWJACK);
  revm_command_add(CMD_ADDGOTO   , cmd_addgoto       , revm_getoption2,   1, HLP_ADDGOTO);
  revm_command_add(CMD_SETGVL    , cmd_setgvl        , revm_getoption,    1, HLP_SETGVL);
  revm_command_add(CMD_RENAME     , cmd_rename        , revm_getoption2,   1, HLP_RENAME);
  revm_command_add(CMD_CONTROL   , cmd_control       , NULL,            1, HLP_CONTROL);

  revm_command_add(CMD_ITRACE, cmd_kedbgitrace, NULL, 0, HLP_ITRACE);
  /*   revm_command_add(CMD_WATCH    , (void *) cmd_watch    , revm_getvarparams, 1, HLP_WATCH); */
  /*   revm_command_add(CMD_STACK    , (void *) cmd_stack    , revm_getoption,    1, HLP_STACK); */
  /*   revm_command_add(CMD_THREADS  , (void *) cmd_threads  , revm_getvarparams, 1, HLP_THREADS); */

}


/**************** Vector registration ****************/
void     kedbg_register_vector(void)
{
  elfsh_register_readmema(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_readmema);
  elfsh_register_readmem(ELFSH_OS_LINUX,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
  elfsh_register_writemem(ELFSH_OS_LINUX, ELFSH_IOTYPE_GDBPROT, kedbg_writemem);
  elfsh_register_readmema(ELFSH_OS_ARM, ELFSH_IOTYPE_GDBPROT, kedbg_readmema);
  elfsh_register_readmem(ELFSH_OS_ARM,  ELFSH_IOTYPE_GDBPROT, kedbg_readmem);
  elfsh_register_writemem(ELFSH_OS_ARM, ELFSH_IOTYPE_GDBPROT, kedbg_writemem);
  e2dbg_register_pregshook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_print_reg_ia32);
  e2dbg_register_pregshook(ELFSH_ARCH_ARM, ELFSH_HOST_GDB, ELFSH_OS_ARM,
			   kedbg_print_reg_ARM);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_getfp_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_getfp_ARM);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			    kedbg_bt_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			    kedbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			    kedbg_getret_ARM);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB,
			   ELFSH_OS_LINUX, kedbg_set_regvars_ia32);
  e2dbg_register_sregshook(ELFSH_ARCH_ARM, ELFSH_HOST_GDB,
			   ELFSH_OS_ARM, kedbg_set_regvars_ARM);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB,
			   ELFSH_OS_LINUX, kedbg_get_regvars_ia32);
  e2dbg_register_gregshook(ELFSH_ARCH_ARM, ELFSH_HOST_GDB,
			   ELFSH_OS_ARM, kedbg_get_regvars_ARM);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB,
			   ELFSH_OS_LINUX, kedbg_getpc_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_ARM, ELFSH_HOST_GDB,
			   ELFSH_OS_ARM, kedbg_getpc_ARM);
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_HOST_GDB, ELFSH_OS_LINUX,
			   kedbg_setbp);
//  e2dbg_register_breakhook(ELFSH_ARCH_ARM, ELFSH_HOST_GDB, ELFSH_OS_ARM,
//			   kedbg_setbp);
  e2dbg_register_delbreakhook(ELFSH_HOST_GDB, kedbg_delbp);
}
