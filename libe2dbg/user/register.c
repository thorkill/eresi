#include "libe2dbg.h"


static void     e2dbg_user_register_hooks(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Linux / AMD64 target */
  e2dbg_register_gregshook(ELFSH_ARCH_AMD64, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_amd64_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_AMD64, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_amd64_sysv);
  e2dbg_register_getpchook(ELFSH_ARCH_AMD64, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_amd64);
  e2dbg_register_setstephook(ELFSH_ARCH_AMD64, ELFSH_HOST_USER, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_amd64);
  e2dbg_register_resetstephook(ELFSH_ARCH_AMD64, ELFSH_HOST_USER, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_amd64);
  e2dbg_register_nextfphook(ELFSH_ARCH_AMD64, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_bt_amd64);
  e2dbg_register_getfphook(ELFSH_ARCH_AMD64, ELFSH_FILE_LIB, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_amd64);
  e2dbg_register_getrethook(ELFSH_ARCH_AMD64, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_getret_amd64);
  e2dbg_register_breakhook(ELFSH_ARCH_AMD64, ELFSH_FILE_LIB,   
			   ELFSH_OS_LINUX, e2dbg_break_amd64);  

  /* Initialize vectors */
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_get_regvars_ia32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_ia32_sysv);
  e2dbg_register_gregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_get_regvars_ia32_sysv);

  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_set_regvars_ia32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_ia32_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_set_regvars_ia32_sysv);

  e2dbg_register_pregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, ELFSH_OS_LINUX, 
			   e2dbg_print_regvars_ia32_sysv);
  e2dbg_register_pregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, ELFSH_OS_SOLARIS, 
			   e2dbg_print_regvars_ia32_sysv);
  e2dbg_register_pregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, ELFSH_OS_FREEBSD, 
			   e2dbg_print_regvars_ia32_freebsd);
  e2dbg_register_pregshook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, ELFSH_OS_NETBSD, 
			   e2dbg_print_regvars_ia32_netbsd);
  e2dbg_register_pregshook(ELFSH_ARCH_MIPS32, ELFSH_HOST_USER, ELFSH_OS_IRIX, 
			   e2dbg_print_regvars_mips_irix);
  e2dbg_register_pregshook(ELFSH_ARCH_MIPS64, ELFSH_HOST_USER, ELFSH_OS_IRIX, 
			   e2dbg_print_regvars_mips_irix);

  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_getpc_bsd_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_ia32);
  e2dbg_register_getpchook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_getpc_sysv_ia32);
  
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			     ELFSH_OS_FREEBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			     ELFSH_OS_NETBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			     ELFSH_OS_OPENBSD, e2dbg_setstep_bsd_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_ia32);
  e2dbg_register_setstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			     ELFSH_OS_SOLARIS, e2dbg_setstep_sysv_ia32);
  
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			       ELFSH_OS_FREEBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			       ELFSH_OS_NETBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			       ELFSH_OS_OPENBSD, e2dbg_resetstep_bsd_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_ia32);
  e2dbg_register_resetstephook(ELFSH_ARCH_IA32, ELFSH_HOST_USER, 
			       ELFSH_OS_SOLARIS, e2dbg_resetstep_sysv_ia32);


  /* For sparc32 */

  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_get_regvars_sparc32_bsd);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_get_regvars_sparc32_sysv);
  e2dbg_register_gregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_get_regvars_sparc32_sysv);


  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_set_regvars_sparc32_bsd);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_set_regvars_sparc32_sysv);
  e2dbg_register_sregshook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_set_regvars_sparc32_sysv);


  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_FREEBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_NETBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_OPENBSD, e2dbg_getpc_bsd_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_LINUX, e2dbg_getpc_sysv_sparc32);
  e2dbg_register_getpchook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			   ELFSH_OS_SOLARIS, e2dbg_getpc_sysv_sparc32);
  
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			     ELFSH_OS_FREEBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			     ELFSH_OS_NETBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			     ELFSH_OS_OPENBSD, e2dbg_setstep_bsd_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			     ELFSH_OS_LINUX, e2dbg_setstep_sysv_sparc32);
  e2dbg_register_setstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			     ELFSH_OS_SOLARIS, e2dbg_setstep_sysv_sparc32);
  
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			       ELFSH_OS_FREEBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			       ELFSH_OS_NETBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			       ELFSH_OS_OPENBSD, e2dbg_resetstep_bsd_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			       ELFSH_OS_LINUX, e2dbg_resetstep_sysv_sparc32);
  e2dbg_register_resetstephook(ELFSH_ARCH_SPARC32, ELFSH_HOST_USER, 
			       ELFSH_OS_SOLARIS, e2dbg_resetstep_sysv_sparc32);


  /***********************************/
  /* Now nextfp hooks                */
  /***********************************/
  
  /* Usual nextfp targets for ET_EXEC/ET_DYN on IA32 */
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_NETBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_ia32);
  e2dbg_register_nextfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_SOLARIS, e2dbg_bt_ia32);
  
  /* Now for sparc */
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_FREEBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_NETBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_OPENBSD, e2dbg_bt_sparc32);
  e2dbg_register_nextfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_SOLARIS, e2dbg_bt_sparc32);



  /************************************/
  /* Now getfp hooks	              */
  /***********************************/
  
  /* Usual getfp targets for ET_EXEC/ET_DYN on IA32 */
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_ia32);
  e2dbg_register_getfphook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_ia32);

  /* Now for sparc */
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			   ELFSH_OS_LINUX, e2dbg_getfp_sysv_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			   ELFSH_OS_FREEBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			   ELFSH_OS_NETBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			   ELFSH_OS_OPENBSD, e2dbg_getfp_bsd_sparc32);
  e2dbg_register_getfphook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			   ELFSH_OS_SOLARIS, e2dbg_getfp_sysv_sparc32);


  /***********************************/
  /* Now register getret hooks       */
  /***********************************/
  
  /* Usual getret targets for ET_EXEC/IA32 */
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_NETBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_ia32);
  e2dbg_register_getrethook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_ia32);
  
  /* Now for sparc32 */
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_LINUX, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_NETBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_LINUX, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_FREEBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_NETBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_OPENBSD, e2dbg_getret_sparc32);
  e2dbg_register_getrethook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB, 
			    ELFSH_OS_SOLARIS, e2dbg_getret_sparc32);

  /***********************************/
  /* Now register breakpoint hooks   */
  /***********************************/
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_LINUX, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_FREEBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_NETBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_OPENBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_SOLARIS, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,   
			   ELFSH_OS_LINUX, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,   
			   ELFSH_OS_FREEBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,   
			   ELFSH_OS_NETBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,   
			   ELFSH_OS_OPENBSD, e2dbg_break_ia32);  
  e2dbg_register_breakhook(ELFSH_ARCH_IA32, ELFSH_FILE_LIB,   
			   ELFSH_OS_SOLARIS, e2dbg_break_ia32);

  /* Now for sparc */
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_LINUX, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_FREEBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_NETBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_OPENBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_EXEC,   
			   ELFSH_OS_SOLARIS, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB,   
			   ELFSH_OS_LINUX, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB,   
			   ELFSH_OS_FREEBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB,   
			   ELFSH_OS_NETBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB,   
			   ELFSH_OS_OPENBSD, e2dbg_break_sparc32);  
  e2dbg_register_breakhook(ELFSH_ARCH_SPARC32, ELFSH_FILE_LIB,   
			   ELFSH_OS_SOLARIS, e2dbg_break_ia32);
  
  /* Now delete break hook */
  e2dbg_register_delbreakhook(ELFSH_HOST_USER, e2dbg_delbreak_user);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


int             e2dbg_user_hooks_install(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  static Bool   done;

  if (!done)
    {
      e2dbg_setup_hooks();
      e2dbg_user_register_hooks();
    }
  done = TRUE;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
