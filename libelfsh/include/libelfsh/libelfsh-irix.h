/*
** libelfsh-irix.h
**
** Various undefined macros on IRIX
**
** Last update Mon Aug 23 05:05:27 2005 ym
**
** $Id$
**
*/

#include <rld_interface.h>
#include <dlfcn.h>

#ifndef STT_TLS
  #define STT_TLS  6
  #undef  STT_NUM
  #define STT_NUM  7
#endif


#define SHT_GNU_verdef	  0x6ffffffd	/* Version definition section.  */
#define SHT_GNU_verneed	  0x6ffffffe	/* Version needs section.  */
#define SHT_GNU_versym	  0x6fffffff	/* Version symbol table.  */



/* Version definition sections.  */

typedef struct
{
  Elf32_Half	vd_version;		/* Version revision */
  Elf32_Half	vd_flags;		/* Version information */
  Elf32_Half	vd_ndx;			/* Version Index */
  Elf32_Half	vd_cnt;			/* Number of associated aux entries */
  Elf32_Word	vd_hash;		/* Version name hash value */
  Elf32_Word	vd_aux;			/* Offset in bytes to verdaux array */
  Elf32_Word	vd_next;		/* Offset in bytes to next verdef
					   entry */
} Elf32_Verdef;

typedef struct
{
  Elf64_Half	vd_version;		/* Version revision */
  Elf64_Half	vd_flags;		/* Version information */
  Elf64_Half	vd_ndx;			/* Version Index */
  Elf64_Half	vd_cnt;			/* Number of associated aux entries */
  Elf64_Word	vd_hash;		/* Version name hash value */
  Elf64_Word	vd_aux;			/* Offset in bytes to verdaux array */
  Elf64_Word	vd_next;		/* Offset in bytes to next verdef
					   entry */
} Elf64_Verdef;


/* Legal values for vd_version (version revision).  */
#define VER_DEF_NONE	0		/* No version */
#define VER_DEF_CURRENT	1		/* Current version */
#define VER_DEF_NUM	2		/* Given version number */

/* Legal values for vd_flags (version information flags).  */
#define VER_FLG_BASE	0x1		/* Version definition of file itself */
#define VER_FLG_WEAK	0x2		/* Weak version identifier */

/* Versym symbol index values.  */
#define	VER_NDX_LOCAL		0	/* Symbol is local.  */
#define	VER_NDX_GLOBAL		1	/* Symbol is global.  */
#define	VER_NDX_LORESERVE	0xff00	/* Beginning of reserved entries.  */
#define	VER_NDX_ELIMINATE	0xff01	/* Symbol is to be eliminated.  */

/* Auxialiary version information.  */

typedef struct
{
  Elf32_Word	vda_name;		/* Version or dependency names */
  Elf32_Word	vda_next;		/* Offset in bytes to next verdaux
					   entry */
} Elf32_Verdaux;

typedef struct
{
  Elf64_Word	vda_name;		/* Version or dependency names */
  Elf64_Word	vda_next;		/* Offset in bytes to next verdaux
					   entry */
} Elf64_Verdaux;


/* Version dependency section.  */

typedef struct
{
  Elf32_Half	vn_version;		/* Version of structure */
  Elf32_Half	vn_cnt;			/* Number of associated aux entries */
  Elf32_Word	vn_file;		/* Offset of filename for this
					   dependency */
  Elf32_Word	vn_aux;			/* Offset in bytes to vernaux array */
  Elf32_Word	vn_next;		/* Offset in bytes to next verneed
					   entry */
} Elf32_Verneed;

typedef struct
{
  Elf64_Half	vn_version;		/* Version of structure */
  Elf64_Half	vn_cnt;			/* Number of associated aux entries */
  Elf64_Word	vn_file;		/* Offset of filename for this
					   dependency */
  Elf64_Word	vn_aux;			/* Offset in bytes to vernaux array */
  Elf64_Word	vn_next;		/* Offset in bytes to next verneed
					   entry */
} Elf64_Verneed;


/* Legal values for vn_version (version revision).  */
#define VER_NEED_NONE	 0		/* No version */
#define VER_NEED_CURRENT 1		/* Current version */
#define VER_NEED_NUM	 2		/* Given version number */

/* Auxiliary needed version information.  */

typedef struct
{
  Elf32_Word	vna_hash;		/* Hash value of dependency name */
  Elf32_Half	vna_flags;		/* Dependency specific information */
  Elf32_Half	vna_other;		/* Unused */
  Elf32_Word	vna_name;		/* Dependency name string offset */
  Elf32_Word	vna_next;		/* Offset in bytes to next vernaux
					   entry */
} Elf32_Vernaux;

typedef struct
{
  Elf64_Word	vna_hash;		/* Hash value of dependency name */
  Elf64_Half	vna_flags;		/* Dependency specific information */
  Elf64_Half	vna_other;		/* Unused */
  Elf64_Word	vna_name;		/* Dependency name string offset */
  Elf64_Word	vna_next;		/* Offset in bytes to next vernaux
					   entry */
} Elf64_Vernaux;


/* Legal values for vna_flags.  */
#define VER_FLG_WEAK	0x2		/* Weak version identifier */

/* pi3 test */
typedef struct
{
  Elf32_Word n_namesz;                  /* Length of the note's name.  */
  Elf32_Word n_descsz;                  /* Length of the note's descriptor.  */
  Elf32_Word n_type;                    /* Type of the note.  */
} Elf32_Nhdr;

typedef struct
{
  Elf64_Word n_namesz;                  /* Length of the note's name.  */
  Elf64_Word n_descsz;                  /* Length of the note's descriptor.  */
  Elf64_Word n_type;                    /* Type of the note.  */
} Elf64_Nhdr;


#ifndef DT_VERDEFNUM
 #define DT_VERDEFNUM   0x6ffffffd
#endif
#ifndef DT_VERNEEDNUM
 #define DT_VERNEEDNUM  0x6fffffff
#endif
#ifndef DT_VERNEED
 #define DT_VERNEED     0x6ffffffe
#endif
#ifndef DT_VERSYM
 #define DT_VERSYM      0x6ffffff0
#endif
#ifndef DT_VERDEF
 #define DT_VERDEF      0x6ffffffc
#endif

#ifndef STT_COMMON              /* undefined on debian */
 #define STT_COMMON     5
#endif

#define NT_PRSTATUS     1               /* Contains copy of prstatus struct */
#define NT_FPREGSET     2               /* Contains copy of fpregset struct */
#define NT_PRPSINFO     3               /* Contains copy of prpsinfo struct */
#define NT_PRXREG       4               /* Contains copy of prxregset struct */
#define NT_TASKSTRUCT   4               /* Contains copy of task structure */
#define NT_PLATFORM     5               /* String from sysinfo(SI_PLATFORM) */
#define NT_AUXV         6               /* Contains copy of auxv array */
#define NT_GWINDOWS     7               /* Contains copy of gwindows struct */
#define NT_ASRS         8               /* Contains copy of asrset struct */
#define NT_PSTATUS      10              /* Contains copy of pstatus struct */
#define NT_PSINFO       13              /* Contains copy of psinfo struct */
#define NT_PRCRED       14              /* Contains copy of prcred struct */
#define NT_UTSNAME      15              /* Contains copy of utsname struct */
#define NT_LWPSTATUS    16              /* Contains copy of lwpstatus struct */
#define NT_LWPSINFO     17              /* Contains copy of lwpinfo struct */
#define NT_PRFPXREG     20              /* Contains copy of fprxregset struct*/

#ifndef SHF_MERGE
 #define SHF_MERGE            (1 << 4)   /* Might be merged */
#endif

#ifndef SHF_STRINGS
 #define SHF_STRINGS          (1 << 5)   /* Contains nul-terminated strings */
#endif

#ifndef SHF_INFO_LINK
 #define SHF_INFO_LINK        (1 << 6)   /* `sh_info' contains SHT index */
#endif

#ifndef SHF_LINK_ORDER
 #define SHF_LINK_ORDER       (1 << 7)   /* Preserve order after combining */
#endif


#define EM_ARM 40


#ifndef DF_P1_LAZYLOAD
 #define DF_P1_LAZYLOAD 0x00000001      /* Lazyload following object.  */
#endif

#ifndef DF_P1_GROUPPERM
 #define DF_P1_GROUPPERM        0x00000002      /* Symbols from next object are not */
#endif

#ifndef DF_ORIGIN
 #define DF_ORIGIN      0x00000001      /* Object may use df_origin */
#endif

#ifndef DF_SYMBOLIC
 #define DF_SYMBOLIC    0x00000002      /* Symbol resolutions starts here */
#endif

#ifndef DF_TEXTREL
 #define DF_TEXTREL     0x00000004      /* Object contains text relocations */
#endif

#ifndef DF_BIND_NOW
 #define DF_BIND_NOW    0x00000008      /* No lazy binding for this object */
#endif

#ifndef DF_1_NOW
 #define DF_1_NOW       0x00000001      /* Set RTLD_NOW for this object.  */
#endif

#ifndef DF_1_GLOBAL
 #define DF_1_GLOBAL    0x00000002      /* Set RTLD_GLOBAL for this object.  */
#endif

#ifndef DF_1_GROUP
 #define DF_1_GROUP     0x00000004      /* Set RTLD_GROUP for this object.  */
#endif

#ifndef DF_1_NODELETE
 #define DF_1_NODELETE  0x00000008      /* Set RTLD_NODELETE for this object.*/
#endif

#ifndef DF_1_LOADFLTR
 #define DF_1_LOADFLTR  0x00000010      /* Trigger filtee loading at runtime.*/
#endif

#ifndef DF_1_INITFIRST
 #define DF_1_INITFIRST 0x00000020      /* Set RTLD_INITFIRST for this object*/
#endif

#ifndef DF_1_NOOPEN
 #define DF_1_NOOPEN    0x00000040      /* Set RTLD_NOOPEN for this object.  */
#endif

#ifndef DF_1_ORIGIN
 #define DF_1_ORIGIN    0x00000080      /* $ORIGIN must be handled.  */
#endif

#ifndef DF_1_DIRECT
 #define DF_1_DIRECT    0x00000100      /* Direct binding enabled.  */
#endif

#ifndef DF_1_TRANS
 #define DF_1_TRANS     0x00000200
#endif

#ifndef DF_1_INTERPOSE
 #define DF_1_INTERPOSE 0x00000400      /* Object is used to interpose.  */
#endif

#ifndef DF_1_NODEFLIB
 #define DF_1_NODEFLIB  0x00000800      /* Ignore default lib search path.  */
#endif

#ifndef DF_1_NODUMP
 #define DF_1_NODUMP    0x00001000
#endif

#ifndef DF_1_CONFALT
 #define DF_1_CONFALT   0x00002000
#endif

#ifndef DF_1_ENDFILTEE
 #define DF_1_ENDFILTEE 0x00004000
#endif

#ifndef DTF_1_PARINIT
 #define DTF_1_PARINIT  0x00000001
#endif

#ifndef DTF_1_CONFEXP
 #define DTF_1_CONFEXP  0x00000002
#endif

#ifndef O_SYNC
 #define O_SYNC O_FSYNC
#endif
#ifndef DT_BIND_NOW
 #define DT_BIND_NOW    24
#endif
#ifndef DT_SYMINENT
 #define DT_SYMINIENT   0x6ffffdff
#endif
#ifndef DT_FLAGS
 #define DT_FLAGS       30
#endif
#ifndef DT_VERDEFNUM
 #define DT_VERDEFNUM   0x6ffffffd
#endif
#ifndef DT_VERNEEDNUM
 #define DT_VERNEEDNUM  0x6fffffff
#endif
#ifndef DT_PLTADSZ
 #define DT_PLTPADSZ    0x6ffffdf9
#endif
#ifndef DT_VERDEF
 #define DT_VERDEF      0x6ffffffc
#endif
#ifndef DT_SYMINENT
 #define DT_SYMINENT    0x6ffffdff
#endif
#ifndef DT_RELACOUNT
 #define DT_RELACOUNT   0x6ffffff9
#endif
#ifndef DT_RELCOUNT
 #define DT_RELCOUNT    0x6ffffffa
#endif
#ifndef DT_SYMINFO
 #define DT_SYMINFO     0x6ffffeff
#endif
#ifndef DT_SYMINSZ
 #define DT_SYMINSZ     0x6ffffdfe
#endif
#ifndef DT_CHECKSUM
 #define DT_CHECKSUM    0x6ffffdf8
#endif
#ifndef DT_VERSYM
 #define DT_VERSYM      0x6ffffff0
#endif
#ifndef DT_MOVESZ
 #define DT_MOVESZ      0x6ffffdfb
#endif
#ifndef DT_VERNEED
 #define DT_VERNEED     0x6ffffffe
#endif
#ifndef DT_FILTER
 #define DT_FILTER      0x7fffffff
#endif
#ifndef DT_MOVEENT
 #define DT_MOVEENT     0x6ffffdfa
#endif
#ifndef DT_AUXILIARY
 #define DT_AUXILIARY   0x7ffffffd
#endif
#ifndef DT_INIT_ARRAY
 #define DT_INIT_ARRAY  25
#endif
#ifndef DT_FINI_ARRAY
 #define DT_FINI_ARRAY  26
#endif
#ifndef DT_INIT_ARRAYSZ
 #define DT_INIT_ARRAYSZ        27
#endif
#ifndef DT_FINI_ARRAYSZ
 #define DT_FINI_ARRAYSZ        28
#endif
#ifndef DT_FLAGS_1
 #define DT_FLAGS_1             0x6ffffffb
#endif
#ifndef DT_FEATURE_1
 #define DT_FEATURE_1           0x6ffffdfc
#endif
#ifndef DT_POSFLAG_1
 #define DT_POSFLAG_1           0x6ffffdfd
#endif

#ifndef DT_RUNPATH
 #define DT_RUNPATH     29              /* Library search path */
#endif

#ifndef DT_ENCODING
 #define DT_ENCODING    32              /* Start of encoded range */
#endif

#ifndef DT_PREINIT_ARRAY
 #define DT_PREINIT_ARRAY 32            /* Array with addresses of preinit fct*/
#endif

#ifndef DT_PREINIT_ARRAYSZ
 #define DT_PREINIT_ARRAYSZ 33          /* size in bytes of DT_PREINIT_ARRAY */
#endif


#ifndef DT_NUM
 #define        DT_NUM          34              /* Number used */
#endif

#if !defined(PT_LOOS)
 #define PT_LOOS         0x60000000      /* Start of OS-specific */
#endif

#if !defined(PT_GNU_EH_FRAME)
 #define PT_GNU_EH_FRAME      0x6474e550
#endif

#define ELF32_ST_VISIBILITY(o)  ((o) & 0x03)


extern char **environ;

/**
 * IRIX doesn't have implementation of function setenv() so we must write own...
 * @param name       - name of environment variable to set
 * @param value      - value for environment variable...
 * @param overwrite  - if overwrite is zero, then the value of name is not changed
 * @return           - return -1 on error
 */

int             setenv(const char *name,
                       const char *value,
                       int overwrite)
{
  char          *ptr=NULL;
  unsigned int  len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* +2 for chars '=' and '\0' */
  len = strlen(name)+strlen(value)+2;

  if (!overwrite)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, ptr, sizeof(char*) * len, -1);
  snprintf(ptr,len,"%s=%s",name,value);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (int)(putenv(ptr)));
}


/**
 * Small wrapper to allow dladdr() usage on IRIX 
 */
int	dladdr(void *address, Dl_info *dl) {
  void	*v;
  v = _rld_new_interface(_RLD_DLADDR,address,dl);
  return (int)v;
}
