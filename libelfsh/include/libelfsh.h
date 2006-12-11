/*
** libelfsh.h for libelfsh
**
** Started on  Mon Jul 23 15:47:12 2001 mayhem
**
*/

#ifndef __LIBELFSH_H_
 #define __LIBELFSH_H_

#include "libvars.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>


#include "elfsh-libc.h"

#include <elf.h>

#include <libelfsh/libelfsh-compat.h>

#ifdef __BEOS__
 #include <bsd_mem.h>
#endif


#define __USE_GNU
#include <sys/ucontext.h>


/* Configure the DEBUG modes for various part of the code */
#define		__DEBUG_MAP__			0
#define		__DEBUG_SECTS__			0
#define		__DEBUG_BLOCKS__		0
#define		__DEBUG_SORT__			0	
#define		__DEBUG_RELADD__		0
#define		__DEBUG_COPYPLT__		0
#define		__DEBUG_BSS__			0
#define		__DEBUG_REDIR__			0
#define		__DEBUG_RUNTIME__		0
#define		__DEBUG_CFLOW__			0
#define		__DEBUG_STATIC__		1
#define		__DEBUG_BREAKPOINTS__		0
#define		__DEBUG_ETRELintoETDYN__	0
#define		__DEBUG_EXTPLT__		0

/* ELFsh architecture types */
#define		ELFSH_ARCH_IA32			0
#define		ELFSH_ARCH_IA64			1
#define		ELFSH_ARCH_SPARC32		2
#define		ELFSH_ARCH_SPARC64		3
#define		ELFSH_ARCH_PARISC		4	/* no hooks yet */
#define		ELFSH_ARCH_PPC32		5	/* no hooks yet */
#define		ELFSH_ARCH_PPC64		6	/* no hooks yet */
#define		ELFSH_ARCH_CRAY			7	/* no hooks yet */
#define		ELFSH_ARCH_ALPHA64		8	
#define		ELFSH_ARCH_MIPS32		9
#define		ELFSH_ARCH_MIPS64		10	/* No hooks yet */
#define         ELFSH_ARCH_ARM                  11
#define         ELFSH_ARCHNUM                   12
#define		ELFSH_ARCH_ERROR		0xFF

/* ELFsh ELF types */
#define		ELFSH_TYPE_EXEC			0
#define		ELFSH_TYPE_DYN			1
#define		ELFSH_TYPENUM			2
#define		ELFSH_TYPE_ERROR		0xFF

/* ELFsh OS types */
#define		ELFSH_OS_LINUX			0
#define		ELFSH_OS_FREEBSD		1
#define		ELFSH_OS_NETBSD			2
#define		ELFSH_OS_OPENBSD		3
#define		ELFSH_OS_SOLARIS		4
#define		ELFSH_OS_UNICOS			5
#define		ELFSH_OS_BEOS			6
#define		ELFSH_OS_IOS			7
#define		ELFSH_OSNUM			8
#define		ELFSH_OS_ERROR			0xFF

/* (k)e2dbg emory hosting types for the debugger hooks */
#define		E2DBG_HOST_KERN		0
#define		E2DBG_HOST_PROC		1
#define		E2DBG_HOSTNUM		2
#define		E2DBG_HOST_ERROR	0xFF

/* libelfsh block hijack types */
#define		ELFSH_HIJACK_TYPE_FLOW		0x000000
#define		ELFSH_HIJACK_TYPE_GOT		0x010000
#define		ELFSH_HIJACK_TYPE_PLT		0x020000

/* The Various relocation stages */
#define		ELFSH_RELOC_STAGE1		1
#define		ELFSH_RELOC_STAGE2		2

/* The builtin hooks vectors list */
#define		ELFSH_HOOK_CFLOW		"hook_cflow"
#define		ELFSH_HOOK_REL			"hook_rel"
#define		ELFSH_HOOK_PLT			"hook_plt"
#define		ELFSH_HOOK_ENCODEPLT		"hook_encodeplt"
#define		ELFSH_HOOK_ENCODEPLT1		"hook_encodeplt1"
#define		ELFSH_HOOK_BREAK		"hook_setbreak"
#define		ELFSH_HOOK_EXTPLT		"hook_extplt"
#define		ELFSH_HOOK_ALTPLT		"hook_altplt"
#define		ELFSH_HOOK_ARGC			"hook_argc"

/* Some defined values */
#define		ELFSH_SECTION_NAME_MAPPED	".mapped"
#define		ELFSH_SECTION_NAME_UNMAPPED	".unmapped"
#define		ELFSH_SECTION_NAME_EXTRAREL	".elfsh.rel"
#define		ELFSH_SECTION_NAME_EXTPLT	".elfsh.extplt"
#define		ELFSH_SECTION_NAME_ALTPLT	".elfsh.altplt"
#define		ELFSH_SECTION_NAME_ALTGOT	".elfsh.altgot"
#define		ELFSH_SECTION_NAME_ALTPLTPROLOG	".elfsh.pltprolog"
#define		ELFSH_SECTION_NAME_ALTGOTPROLOG	".elfsh.gotprolog"
#define		ELFSH_SECTION_NAME_START	".elfsh.start"
#define		ELFSH_SECTION_NAME_PADGOT	".elfsh.padgot"
#define		ELFSH_SECTION_NAME_HOOKS	".elfsh.hooks"
#define		ELFSH_SECTION_NAME_ALTDYNSYM	".elfsh.dynsym"
#define		ELFSH_SECTION_NAME_ALTDYNSTR	".elfsh.dynstr"
#define		ELFSH_SECTION_NAME_ALTRELPLT	".elfsh.relplt"
#define		ELFSH_SECTION_NAME_PADPAGE	".elfsh.padpage"
#define		ELFSH_SECTION_NAME_CONTROL	".elfsh.control"

//#define		ELFSH_SECTION_NAME_RPHT		".elfsh.rpht"
#define		ELFSH_SECTION_NAME_RSHSTRTAB	".rshstrtab"
#define		ELFSH_SECTION_NAME_RSYMTAB	".rsymtab"

/* We need this for string based research, and for sht reconstruction */
#define		ELFSH_SECTION_NAME_NULL		""
#define		ELFSH_SECTION_NAME_PLT		".plt"
#define		ELFSH_SECTION_NAME_DTORS	".dtors"
#define		ELFSH_SECTION_NAME_CTORS	".ctors"
#define		ELFSH_SECTION_NAME_GOT		".got"
#define		ELFSH_SECTION_NAME_RELPLT	".rel.plt"
#define		ELFSH_SECTION_NAME_RELAPLT	".rela.plt"

#define		ELFSH_SECTION_NAME_RELGOT	".rel.got"
#define		ELFSH_SECTION_NAME_RELDYN	".rel.dyn"
#define		ELFSH_SECTION_NAME_ALTRELGOT	".elfsh.reldyn"

#define		ELFSH_SECTION_NAME_GOTPLT	".got.plt"
#define		ELFSH_SECTION_NAME_INTERP	".interp"
#define		ELFSH_SECTION_NAME_SYMTAB	".symtab"
#define		ELFSH_SECTION_NAME_STRTAB	".strtab"
#define		ELFSH_SECTION_NAME_DYNSTR	".dynstr"
#define		ELFSH_SECTION_NAME_SHSTRTAB	".shstrtab"
#define		ELFSH_SECTION_NAME_BSS		".bss"
#define		ELFSH_SECTION_NAME_TEXT		".text"
#define		ELFSH_SECTION_NAME_DATA		".data"
#define		ELFSH_SECTION_NAME_NOTES	".notes"
#define		ELFSH_SECTION_NAME_NOTES_ABI	".notes.ABI-tag"
#define		ELFSH_SECTION_NAME_DYNAMIC	".dynamic"
#define		ELFSH_SECTION_NAME_COMMENT	".comment"
#define		ELFSH_SECTION_NAME_RODATA	".rodata"
#define		ELFSH_SECTION_NAME_STAB		".stab"
#define		ELFSH_SECTION_NAME_EHFRAME	".eh_frame"
#define		ELFSH_SECTION_NAME_INIT		".init"
#define		ELFSH_SECTION_NAME_FINI		".fini"

/* Section index in the secthash */
#define		ELFSH_SECTION_NULL		0
#define		ELFSH_SECTION_INTERP		1
#define		ELFSH_SECTION_NOTE		2
#define		ELFSH_SECTION_HASH		3
#define		ELFSH_SECTION_DYNSYM		4
#define		ELFSH_SECTION_DYNSTR		5
#define         ELFSH_SECTION_GNUVERSYM         6
#define         ELFSH_SECTION_GNUVERNEED        7
#define         ELFSH_SECTION_GNUVERDEF         8
#define		ELFSH_SECTION_INIT		10
#define		ELFSH_SECTION_PLT		11
#define		ELFSH_SECTION_TEXT		12
#define		ELFSH_SECTION_FINI		13
#define		ELFSH_SECTION_RODATA		14
#define		ELFSH_SECTION_DATA		15
#define		ELFSH_SECTION_CTORS		17
#define		ELFSH_SECTION_DTORS		18
#define		ELFSH_SECTION_GOT		19
#define		ELFSH_SECTION_DYNAMIC		20
#define		ELFSH_SECTION_BSS		21
#define		ELFSH_SECTION_STAB		22
#define		ELFSH_SECTION_COMMENT		23
#define		ELFSH_SECTION_SHSTRTAB		24
#define		ELFSH_SECTION_SYMTAB		25
#define		ELFSH_SECTION_STRTAB		26
#define		ELFSH_SECTION_STABSTR		27
#define		ELFSH_SECTION_ALTPLT		28
#define		ELFSH_SECTION_PADGOT		28 /* shared */
#define		ELFSH_SECTION_ALTPLTPROLOG	29
#define		ELFSH_SECTION_ALTGOTPROLOG	29 /* shared */
#define		ELFSH_SECTION_ALTGOT		30
#define		ELFSH_SECTION_GOTPLT		31
#define		ELFSH_SECTION_MIPSTART		32
#define		ELFSH_SECTION_EXTPLT		33
#define		ELFSH_SECTION_RSHSTRTAB		34
#define		ELFSH_SECTION_RSYMTAB		35
#define		ELFSH_SECTION_MAX		254
#define		ELFSH_SECTION_UNKNOWN		255


/* Some constants */
#define		ELFSH_PLT_ENTRY_SIZE	        16
#define		ELFSH_SYMTAB_ENTRY_SIZE		sizeof(elfsh_Sym)

#define		ELFSH_STARTSYM			"_start"
#define		ELFSH_GOTSYM			"_GLOBAL_OFFSET_TABLE_"
#define		ELFSH_NULL_STRING		"(NULL)"
#define		ELFSH_SORT_BY_ADDR		'a'
#define		ELFSH_SORT_BY_SIZE		's'

/* Dedicated to you know who */
#define		ELFSH_MEANING			42

#define		ELFSH_SECTION_FIRST		0
#define		ELFSH_SECTION_LAST		0xFFFF
#define		ELFSH_BLOCK_FIRST		0
#define		ELFSH_BLOCK_LAST		0xFFFF

#define		ELFSH_SPARC_LOWADDR		0x2000
#define		ELFSH_INVALID_ADDR		0xFFFFFFFF
#define		ELFSH_NOLIMIT			0

#define		ELFSH_END_CTORS			((elfsh_Addr) 0xFFFFFFFF)
#define		ELFSH_END_DTORS		        ELFSH_END_CTORS

#define		ELFSH_SHIFTING_ABSENT		0
#define		ELFSH_SHIFTING_NONE		1
#define		ELFSH_SHIFTING_MIPSPLT		2
#define		ELFSH_SHIFTING_PARTIAL		3
#define		ELFSH_SHIFTING_COMPLETE		4

#define		ELFSH_UNSET_OFFSET		(1 << 30)

#define		ELFSH_LOWSYM			1
#define		ELFSH_HIGHSYM			2
#define		ELFSH_EXACTSYM			3

#define		ELFSH_DATA_INJECTION		0
#define		ELFSH_CODE_INJECTION		1
#define		ELFSH_UNMAPPED_INJECTION	2
#define		ELFSH_UNKNOWN_INJECTION		3

#define		ELFSH_TRACE_MAX_ARGS		30

#define		STT_BLOCK			(STT_NUM + 1)


#define	DUMPABLE(sym)		(elfsh_get_symbol_type(sym) == STT_FUNC   || \
				 elfsh_get_symbol_type(sym) == STT_OBJECT || \
				 elfsh_get_symbol_type(sym) == STT_COMMON || \
				 elfsh_get_symbol_type(sym) == STT_SECTION)

#define	FILE_IS_SPARC(obj)	(FILE_IS_SPARC32(obj) || FILE_IS_SPARC64(obj))
#define FILE_IS_SPARC32(obj)	(((elfsh_get_arch((obj)->hdr) == EM_SPARC) || \
				(elfsh_get_arch((obj)->hdr) == EM_SPARC32PLUS)) ? 1 : 0)

#define FILE_IS_SPARC64(obj)	((elfsh_get_arch((obj)->hdr) == EM_SPARCV9) ? 1 : 0)

#define	FILE_IS_MIPS(obj)	(elfsh_get_arch((obj)->hdr) == EM_MIPS || \
				 elfsh_get_arch((obj)->hdr) == EM_MIPS_RS3_LE ? 1 : 0)

#define	FILE_IS_IA64(obj)	((elfsh_get_arch((obj)->hdr)) == EM_IA_64 ? 1 : 0)
#define	FILE_IS_IA32(obj)	((elfsh_get_arch((obj)->hdr)) == EM_386 ? 1 : 0)

#define	FILE_IS_ALPHA64(obj)	((elfsh_get_arch((obj)->hdr)) == EM_ALPHA || \
				(elfsh_get_arch((obj)->hdr)) == EM_ALPHA_EXP ? 1 : 0)
			      
#define IS_REL(scn)		(FILE_IS_SPARC((scn)->parent)   || \
				 FILE_IS_SPARC64((scn)->parent) || \
				 FILE_IS_IA64((scn)->parent)    || \
				 FILE_IS_ALPHA64((scn)->parent) ? 0 : 1)


#define ELFSH_SELECT_INJECTION(a, b, d)	\
		do {	\
    		if ((elfsh_get_ostype(a) == ELFSH_OS_FREEBSD || \
    		    elfsh_get_ostype(a) == ELFSH_OS_BEOS || \
		    elfsh_get_ostype(a) == ELFSH_OS_SOLARIS || \
    		    FILE_IS_ALPHA64(a) || \
		    FILE_IS_MIPS(a) || \
    		    FILE_IS_SPARC(a)) || (b)) { \
    		    d = ELFSH_DATA_INJECTION; \
		} else { \
		    d = ELFSH_CODE_INJECTION; \
		}\
	    } while (0)

/* Old Pax flags (to be read in elfhdr.e_flags) */
#define		ELFSH_PAX_PAGEEXEC         1    /* Paging based non-exec pages */
#define		ELFSH_PAX_EMULTRAMP        2    /* Emulate trampolines */
#define		ELFSH_PAX_MPROTECT         4    /* Restrict mprotect() */
#define		ELFSH_PAX_RANDMMAP         8    /* Randomize mmap() base */
#define		ELFSH_PAX_RANDEXEC         16   /* Randomize ET_EXEC base */
#define	        ELFSH_PAX_SEGMEXEC         32   /* Segmentation based non-exec pages */



/* EI_RPHT is not PaX related */
#define		EI_PAX			   14   /* Index in e_ident[] where to read flags */
#define		EI_RPHT			   10	/* Index in e_ident[] where to read rphtoff */



/* ELFsh redirection abstract unit */
typedef struct	s_redir
{
#define		ELFSH_REDIR_CFLOW	0
#define		ELFSH_REDIR_ALTPLT	1
#define		ELFSH_REDIR_ALTGOT	2
  u_char	type;					/* Redirection type                */

#define		ELFSH_ORIG_FUNC	        0
#define		ELFSH_HOOK_FUNC		1
  char		*name[2];				/* Name of involved functions      */
  elfsh_Addr	 addr[2];				/* Redirection address             */
}		elfshredir_t;



/* ELFsh private stab entry */
typedef struct		s_stab
{
  elfsh_Addr		strindex;
  unsigned char		type;
  unsigned char		other;
  unsigned short	desc;
  elfsh_Addr		value;
}			elfshstabent_t;



/* ELFsh private .notes entry format */
typedef struct		s_nentry
{
  int			namesz;
  int			descsz;
  int			type;
  char			*note;
  char			*desc;
  struct s_nentry	*next;
  struct s_nentry	*prev;
}			elfshnotent_t;


/* Version structures */
typedef struct        s_hashneed
{
  elfsh_Verneed       *need;
  elfsh_Vernaux       *aux;
}		      hashneed_t;

typedef struct        s_hashdef
{
  void                *ps;
  elfsh_Verdef        *def;
  elfsh_Word          *aux;
}                     hashdef_t;


/* ELFsh private relocation format */
typedef struct		s_rel
{
  u_int			idx_src;
  u_int			off_src;
  u_int			idx_dst;
  u_int			off_dst;

#define	ELFSH_RELOC_SECTBASE		0		/* The dword is to be patched depending on section's base */
#define	ELFSH_RELOC_FP			0xFFFFFFFF	/* This entry is a false positive : do not relocate */
  u_int			type;
}			elfshrel_t;

/* forward declaration */
struct			s_sect;
struct			s_obj;
typedef struct s_sect elfshsect_t;
typedef struct s_obj  elfshobj_t;


/* Section data type */
struct			s_sect
{

  /* Filled at creation */
  char			*name;		/* Cached name							*/
  struct s_obj		*parent;	/* Parent Object						*/
  elfsh_Phdr		*phdr;		/* the first parent segment					*/

  /* Filled at creation, can be modified when SHT is sorted */
  elfsh_Shdr		*shdr;		/* section header for this section				*/
  int			index;		/* Section index in sht						*/
  struct s_sect		*next;		/* Next section in the list					*/
  struct s_sect		*prev;		/* Prev section in the list					*/
  
  /* ELFsh section state */
#define		ELFSH_SECTION_ANALYZED		(1 << 0)
#define		ELFSH_SECTION_INSERTED		(1 << 1)
#define		ELFSH_SECTION_MOVED		(1 << 2)
#define		ELFSH_SECTION_REMOVED		(1 << 3)
#define		ELFSH_SECTION_RUNTIME		(1 << 4)
  char			flags;		/* Analysed/Raw, Orphelin/Inserted, Moved, Removed, Runtime/Static */

  /* Filled at loading */
  void			*data;		/* Section's data cache in file					*/

  /* Points in our own address space */
  void			*pdata;		/* Section's data in the process itself				*/

  /* Filled at analysing */
  void			*altdata;	/* Type dependant internal format				*/
  void			*terdata;	/* Alternate type dependant internal format			*/

  /* Filled at 'unrelocation' : ELFsh private relocs fmt only used in experimental features */
  elfshrel_t		*rel;		/* ELFsh private relocation table for this section		*/
  u_int			srcref;		/* Number of absolute reference FROM this section : sizeof(rel)	*/
  u_int			dstref;		/* Number of absolute reference TO this section			*/

  /* Changed during extension */
  u_int			curend;		/* Current real end of section : we save room when possible */

};


/* Link map structure */
typedef struct		s_link_map
{

  elfsh_Addr		laddr;		/* Base address shared object is loaded at. */
  char			*lname;		/* Absolute file name object was found in.  */
  elfsh_Dyn		*lld;		/* Dynamic section of the shared object.    */
  struct s_link_map	*lnext;		/* Next object in linkmap		    */
  struct s_link_map	*lprev;		/* Prev object in linkmap		    */

}			elfshlinkmap_t;


/* The runtime ELF header (unofficial ELF extension) */
/* This structure has been introduced since the Embedded ELF Debugger */
typedef struct	s_rehdr
{
  uint16_t	 rphtnbr;		/* Number of entries in RPHT */
  uint16_t	 rshtnbr;		/* Number of entries in RSHT */
  elfsh_Addr	 base;			/* Base address (runtime ET_DYN object base) */

}		elfshrhdr_t;



/* ELF object structure */
struct		 s_obj
{
  elfsh_Ehdr	 *hdr;				/* Elf header */
  elfsh_Shdr	 *sht;				/* Section header table */
  elfsh_Phdr	 *pht;				/* Program Header table */
  elfshsect_t	 *sectlist;			/* Section linked list */

  elfshrhdr_t	 rhdr;				/* The Runtime header */
  elfsh_Shdr	 *rsht;				/* The Runtime SHT */
  elfsh_Phdr	 *rpht;				/* The Runtime PHT */
  elfshsect_t	 *rsectlist;			/* Runtime Section linked list */

  elfshsect_t	 *secthash[ELFSH_SECTION_MAX];	/* Section hash table (common) */

  int		 fd;			/* File descriptor for the original file */
  char		 *name;			/* Object path */
  struct stat	 fstat;			/* File stat */
  int		 type;			/* ELFSH_OBJECT_CORE, ELFSH_OBJECT_SHARED, ELFSH_OBJECT_RELOC or ELFSH_OBJECT_EXEC */
  int		 rights;		/* 0 = RO, 1 = WR */
  time_t	 loadtime;		/* Time of Loading */
  u_int		 id;			/* Object ID */

  char		 running;		/* Is the process running ? */
  char		 scanned;		/* Has the object already been block scanned ? */
  char		 hdr_broken;		/* Is the header broken/corrupted ? */
  char		 read;			/* Has the object already been read ? */
  char		 shtrm;			/* Mark SHT and Unmapped sections as stripped ? */
  char		 strip;			/* Mark file as stripped */
  char		 pending;		/* Beeing injected */
  uint32_t	 nbrm;			/* Number of section headers to remove at saving */

  char		 shtrb;			/* Reconstruct the SHT if non present */

#define		 ELFSH_MAXREL	40	/* Maximum number of injected ET_REL, change it ! */
  struct s_obj	 *listrel[ELFSH_MAXREL];/* Array of injected ET_REL in this object */
  u_char	 nbrel;			/* Number of injected ET_REL in this object */
  
  char		 buff[ELFSH_MEANING];	/* Internal buffer, sometimes used to avoid a malloc */
  const char	 *error;		/* Last error string */
  struct s_obj	 *next;			/* The list is simply linked */

  hash_t	 redir_hash;		/* Redirections hash table */
  elfshlinkmap_t *linkmap;		/* Linkmap */

  /* 
  ** Every object can have childs
  ** The number 0 is used as a seperator:
  ** 1: (Father)
  ** |-> 101 Child1
  ** |-> ....
  ** |-> 1012 Child12
  ** 
  ** Max childs for a father should be 99, then we can control a strange id like 101001 (1 => 10 => 1)
  */
#define 	ELFSH_CHILD_BASE(o) 	(o->id * 100 * (o->lastchildid > 9 ? 10 : 1))
#define 	ELFSH_CHILD_NEW(o) 	ELFSH_CHILD_BASE(o) + ++o->lastchildid
#define 	ELFSH_CHILD_MAX 	99
  hash_t	 child_hash;		/* Childs hash table */
  hash_t	 parent_hash;		/* Immediate parent hash table */
  hash_t	 root_hash;		/* Root ELF objects for this file */
  int		 lastchildid;	      	/* Last child id */
};



/* The library has its world itself */
/* There stands all the flags that are used by libelfsh */
typedef struct	s_libstate
{

#define		LIBELFSH_MODE_UNKNOWN	0 
#define		LIBELFSH_MODE_STATIC	1
#define		LIBELFSH_MODE_E2DBG	2
  u_char	mode;				/* The current working mode (ondisk/memory) */
  u_char	indebug;			/* 1 when inside the debugger */

#define		ELFSH_NOPROF	0
#define		ELFSH_ERRPROF	1
#define		ELFSH_OUTPROF	2
#define		ELFSH_DEBUGPROF	3
  char		proflevel;	  		/* libelfsh profiling switch */
  int           (*profile)(char *);		/* Profiling output function */
  int           (*profile_err)(char *);		/* Profiling output (error) function */
  
  /* libui pointers */
  void	     	(*endline)();
  /* Simple */
  char		*(*colorinstr)(char *text);
  char        	*(*colorstr)(char *t);
  char         	*(*colorfieldstr)(char *t);
  char         	*(*colortypestr)(char *t);
  char		*(*colorend)(char *text); 
  char		*(*colorwarn)(char *text);
  /* Advanced */
  char 		*(*coloradv)(char *ty, char *p, char *te);
  char		*(*colorinstr_fmt)(char* p, char *t);
  char          *(*coloraddress)(char *p, elfsh_Addr a);
  char          *(*colornumber)(char *p, u_int n);
  char          *(*colorstr_fmt)(char *p, char *t);
  char          *(*colorfieldstr_fmt)(char *p, char *t);
  char          *(*colortypestr_fmt)(char *p, char *t);
  char		*(*colorwarn_fmt)(char *pattern, char *text);
}		libworld_t;



/* Elfsh break points */
typedef struct	s_bp 
{

  /* XXX: To comment ! */
#define		UNDEF	0
#define		INSTR	1
  u_char	type;

  u_char	savedinstr[16];	/* Saved bytes at breakpoints addr */
  char		*cmd[10];	/* Commands to be executed on hit */
  char		cmdnbr;		/* Next available cmd slot */
  elfsh_Addr	addr;		/* Adresse of breakpoint */
  uint32_t	id;		/* Breakpoint ID */
  char		*symname;	/* Symbol match, if any */
  elfshobj_t	*obj;		/* Parent object of breakpoint */

#define	ELFSH_BP_BREAK	0	/* Classical breakpoint */
#define	ELFSH_BP_WATCH	1	/* Do not get back to debugger */
  u_char	flags;		/* Breakpoint flags */

}		elfshbp_t;



/* Extern data */
extern libworld_t	dbgworld;

/* Libelfsh error message */
extern char	*elfsh_error_str;

/* Profiling depth */
extern int	elfsh_depth;


/*
**
** Libelfsh API prototypes
**
** Check elfsh/doc/libelfsh-api.txt for a complete description
**
** XXX: libelfsh-api.txt not updated to 0.5 (it is for 0.43b)
** FIXME: Maybe doxygen soon ? ;)
*/

/* dynamic.c */
elfsh_Dyn	*elfsh_get_dynamic(elfshobj_t *file, int *num);
elfsh_Dyn	*elfsh_get_dynamic_entry_by_type(elfshobj_t *file, elfsh_Word type);
elfsh_Sword	elfsh_get_dynentry_type(elfsh_Dyn *d);
int		elfsh_set_dynentry_type(elfsh_Dyn *d, elfsh_Addr tag);
elfsh_Word	elfsh_get_dynentry_val(elfsh_Dyn *d);
int		elfsh_set_dynentry_val(elfsh_Dyn *d, elfsh_Addr val);
elfsh_Dyn	*elfsh_get_dynamic_entry_by_index(elfsh_Dyn *dynamic, elfsh_Addr index);
char		*elfsh_get_dynentry_string(elfshobj_t *file, elfsh_Dyn *ent);
int		elfsh_endianize_dynamic(elfshsect_t *_new);
int		elfsh_shift_dynamic(elfshobj_t *file, u_int size);

/* symbol.c */
elfsh_Sym	*elfsh_get_symbol_by_name(elfshobj_t *file, char *name);
char		*elfsh_reverse_symbol(elfshobj_t *file, elfsh_Addr sym_value, elfsh_SAddr *offset);
char		*elfsh_get_symbol_name(elfshobj_t *file, elfsh_Sym *s);
void		*elfsh_get_symtab(elfshobj_t *file, int *num);
elfsh_Sym	*elfsh_get_symbol_by_value(elfshobj_t *file, elfsh_Addr vaddr, int *off, int mode);
int		elfsh_strip(elfshobj_t *file);
int		elfsh_set_symbol_name(elfshobj_t *file, elfsh_Sym *s, char *name);
int		elfsh_shift_symtab(elfshobj_t *file, elfsh_Addr lim, int inc);
int		elfsh_insert_sectsym(elfshobj_t *file, elfshsect_t *sect);
int		elfsh_get_symbol_foffset(elfshobj_t *file, elfsh_Sym *sym);
int		elfsh_insert_symbol(elfshsect_t *sect, elfsh_Sym *sym, char *name);
int		elfsh_insert_funcsym(elfshobj_t *file, char *name, elfsh_Addr vaddr, 
				     uint32_t sz, uint32_t sctidx);

/* dynsym.c */
elfsh_Sym	*elfsh_get_dynsymbol_by_name(elfshobj_t *file, char *name);
void		*elfsh_get_dynsymtab(elfshobj_t *file, int *num);
char		*elfsh_reverse_dynsymbol(elfshobj_t *file, elfsh_Addr sym_value, elfsh_SAddr *offset);
char		*elfsh_get_dynsymbol_name(elfshobj_t *file, elfsh_Sym *s);
int		elfsh_set_dynsymbol_name(elfshobj_t *file, elfsh_Sym *s, char *name);
int		elfsh_shift_dynsym(elfshobj_t *file, elfsh_Addr limit, int inc);

/* sym_common.c */
elfsh_Sym	elfsh_create_symbol(elfsh_Addr value, int size, int type, int binding, int vis, int idx);
elfsh_Sym	*elfsh_copy_symtab(void *addr, int size);
elfsh_Sym	*elfsh_merge_symtabs(elfsh_Sym *one, elfsh_Sym *two, int size_one, int size_two);
elfsh_Sym	*elfsh_get_sym_by_value(elfsh_Sym *sym, int num, elfsh_Addr vaddr, int *off, int mode);
elfsh_Word      elfsh_get_symbol_size(elfsh_Sym *s);
int		elfsh_set_symbol_size(elfsh_Sym *s, elfsh_Addr size);
elfsh_Addr      elfsh_get_symbol_value(elfsh_Sym *s);
int		elfsh_set_symbol_value(elfsh_Sym *s, elfsh_Addr value);
u_char		elfsh_get_symbol_bind(elfsh_Sym *s);
u_char		elfsh_set_symbol_bind(elfsh_Sym *s, elfsh_Addr opt);
u_char		elfsh_get_symbol_type(elfsh_Sym *s);
u_char		elfsh_set_symbol_type(elfsh_Sym *s, elfsh_Addr type);
u_char		elfsh_get_symbol_visibility(elfsh_Sym *s);
u_int		elfsh_set_symbol_visibility(elfsh_Sym *s, elfsh_Addr vis);
u_int		elfsh_get_symbol_link(elfsh_Sym *s);
u_int		elfsh_set_symbol_link(elfsh_Sym *s, elfsh_Addr val);
elfsh_Sym	*elfsh_get_symbol_by_index(elfsh_Sym *symtab, elfsh_Addr index);
elfsh_Sym	*elfsh_get_metasym_by_name(elfshobj_t *file, char *name);
char		*elfsh_reverse_metasym(elfshobj_t *file, elfsh_Addr vaddr, elfsh_SAddr *off);
int		elfsh_remove_symbol(elfshsect_t *symtab, char *name);
void		elfsh_symtab_endianize(elfshsect_t *tab);
int		elfsh_endianize_symtab(elfshsect_t *tab);
int		elfsh_shift_syms(elfshobj_t *file, elfshsect_t *symtab, 
				 elfsh_Addr limit, int inc);

/* obj.c */
elfshobj_t	*elfsh_load_obj(char *name);
void		elfsh_unload_obj(elfshobj_t *file);

/* elf.c */
int		elfsh_load_hdr(elfshobj_t *file);
void		*elfsh_get_hdr(elfshobj_t *file);
elfsh_Off       elfsh_get_shtoff(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_shtnbr(elfsh_Ehdr *e);
elfsh_Off       elfsh_get_phtoff(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_phtnbr(elfsh_Ehdr *e);
elfsh_Addr      elfsh_get_entrypoint(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_arch(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_objtype(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_shstrtab_index(elfsh_Ehdr *e);
elfsh_Word      elfsh_get_version(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_ehsize(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_phentsize(elfsh_Ehdr *e);
elfsh_Half      elfsh_get_shentsize(elfsh_Ehdr *e);
int             elfsh_set_shtoff(elfsh_Ehdr *e, elfsh_Addr off);
int             elfsh_set_shtnbr(elfsh_Ehdr *e, elfsh_Addr num);
int             elfsh_set_phtoff(elfsh_Ehdr *e, elfsh_Addr num);
int		elfsh_set_phtnbr(elfsh_Ehdr *e, elfsh_Addr num);
int             elfsh_set_entrypoint(elfsh_Ehdr *e, elfsh_Addr addr);
u_int		elfsh_set_arch(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_objtype(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_shstrtab_index(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_version(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_ehsize(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_phentsize(elfsh_Ehdr *e, elfsh_Addr val);
u_int		elfsh_set_shentsize(elfsh_Ehdr *e, elfsh_Addr val);
int		elfsh_get_encoding(elfsh_Ehdr *hdr);
int		elfsh_set_encoding(elfsh_Ehdr *hdr, elfsh_Addr type);
int		elfsh_set_flags(elfsh_Ehdr *hdr, elfsh_Addr flags);
elfsh_Word	elfsh_get_flags(elfsh_Ehdr *hdr);
int		elfsh_set_magic(elfsh_Ehdr *hdr, elfsh_Addr mag);
int		elfsh_get_magic(elfsh_Ehdr *hdr);
int		elfsh_set_class(elfsh_Ehdr *hdr, elfsh_Addr _class);
int		elfsh_get_class(elfsh_Ehdr *hdr);
void		elfsh_endianize_elfhdr(elfsh_Ehdr *e, char byteorder);

int		elfsh_static_file(elfshobj_t *file);
int		elfsh_dynamic_file(elfshobj_t *file);
int		elfsh_set_rphtoff(elfsh_Ehdr *hdr, elfsh_Addr num);
elfsh_Off	elfsh_get_rphtoff(elfsh_Ehdr *hdr);


/* pax.c */
int		elfsh_set_paxflags(elfsh_Ehdr *hdr, elfsh_Addr flags);
elfsh_Word	elfsh_get_paxflags(elfsh_Ehdr *hdr);
char            elfsh_set_pax_pageexec(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_pageexec(elfsh_Ehdr *hdr);
char            elfsh_set_pax_emultramp(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_emultramp(elfsh_Ehdr *hdr);
char            elfsh_set_pax_mprotect(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_mprotect(elfsh_Ehdr *hdr);
char            elfsh_set_pax_randmmap(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_randmmap(elfsh_Ehdr *hdr);
char            elfsh_set_pax_randexec(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_randexec(elfsh_Ehdr *hdr);
char            elfsh_set_pax_segmexec(elfsh_Ehdr *hdr, elfsh_Addr off);
char            elfsh_get_pax_segmexec(elfsh_Ehdr *hdr);


/* stab.c */
void		*elfsh_get_stab(elfshobj_t *file, int *num);
elfsh_Addr	elfsh_get_stab_offset(elfshstabent_t *s);
char		*elfsh_get_stab_name(elfshobj_t *file, elfshstabent_t *s);
u_int		elfsh_get_stab_type(elfshstabent_t *s);


/* sht.c */
elfsh_Shdr      *elfsh_get_shtentry_from_sym(elfshobj_t *file, elfsh_Sym *sym);
void		*elfsh_get_sht(elfshobj_t *file, int *num);
void		*elfsh_get_runtime_sht(elfshobj_t *file, int *num);
int		elfsh_load_sht(elfshobj_t *file);
elfsh_Sym	*elfsh_get_sym_from_shtentry(elfshobj_t *file, elfsh_Shdr *hdr);
elfsh_Word      elfsh_get_section_info(elfsh_Shdr *s);
elfsh_Word      elfsh_get_section_entsize(elfsh_Shdr *s);
elfsh_Word      elfsh_get_section_link(elfsh_Shdr *s);
elfsh_Off       elfsh_get_section_foffset(elfsh_Shdr *s);
elfsh_Addr      elfsh_get_section_addr(elfsh_Shdr *s);
elfsh_Word      elfsh_get_section_align(elfsh_Shdr *s);
elfsh_Word      elfsh_get_section_size(elfsh_Shdr *s);
elfsh_Word      elfsh_get_section_type(elfsh_Shdr *s);
int             elfsh_set_section_info(elfsh_Shdr *s, elfsh_Addr info);
int             elfsh_set_section_entsize(elfsh_Shdr *s, elfsh_Addr entsize);
int             elfsh_set_section_link(elfsh_Shdr *s, elfsh_Addr link);
int		elfsh_set_section_foffset(elfsh_Shdr *s, elfsh_Addr offset);
int		elfsh_set_section_addr(elfsh_Shdr *s, elfsh_Addr addr);
int		elfsh_set_section_align(elfsh_Shdr *s, elfsh_Addr align);
int		elfsh_set_section_size(elfsh_Shdr *s, elfsh_Addr size);
int		elfsh_set_section_type(elfsh_Shdr *s, elfsh_Addr type);
char		elfsh_get_section_execflag(elfsh_Shdr *s);
char		elfsh_get_section_writableflag(elfsh_Shdr *s);
char		elfsh_get_section_allocflag(elfsh_Shdr *s);
char		elfsh_get_section_mergeableflag(elfsh_Shdr *s);
char		elfsh_get_section_strflag(elfsh_Shdr *s);
char		elfsh_get_section_linkflag(elfsh_Shdr *s);
char		elfsh_get_section_orderflag(elfsh_Shdr *s);
char		elfsh_set_section_execflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_writableflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_allocflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_mergeableflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_strflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_linkflag(elfsh_Shdr *s, elfsh_Addr val);
char		elfsh_set_section_orderflag(elfsh_Shdr *s, elfsh_Addr val);
int		elfsh_insert_shdr(elfshobj_t *file, elfsh_Shdr hdr, u_int range, char *name, char shift);
void		elfsh_remove_sht(elfshobj_t *file);
int		elfsh_set_section_name(elfshobj_t *file, elfshsect_t *sect, char *name);
char		*elfsh_get_section_name(elfshobj_t *file, elfshsect_t *s);
elfsh_Shdr	elfsh_create_shdr(elfsh_Word name, elfsh_Word type, elfsh_Word flags,
				  elfsh_Addr addr, elfsh_Off offset, elfsh_Word size,
				  elfsh_Word link, elfsh_Word info, elfsh_Word align,
				  elfsh_Word entsize);
int		elfsh_insert_runtime_shdr(elfshobj_t *f, elfsh_Shdr hdr, u_int r, char *name, char sf);

int		elfsh_sort_sht(elfshobj_t *file);
void		elfsh_sync_sectnames(elfshobj_t *file);
void		elfsh_sync_sht(elfshobj_t *file);
elfsh_Shdr	*elfsh_get_sht_entry_by_index(elfsh_Shdr *s, elfsh_Addr index);
elfsh_Shdr	*elfsh_get_sht_entry_by_name(elfshobj_t *file, char *name);
int		elfsh_endianize_sht(elfsh_Shdr *sht, char byteorder, uint16_t shnum);
int		elfsh_filter_sht(elfshobj_t *file);

/* pht.c */
void		*elfsh_get_pht(elfshobj_t *file, int *num);
elfsh_Phdr	*elfsh_get_segment_by_type(elfshobj_t *file, int type, int range);
elfsh_Phdr	*elfsh_get_parent_segment(elfshobj_t *file, elfshsect_t *_new);
int		elfsh_segment_is_parent(elfshsect_t *_new, elfsh_Phdr *p);
int		elfsh_load_pht(elfshobj_t *file);
int		elfsh_segment_is_writable(elfsh_Phdr *p);
int		elfsh_segment_is_readable(elfsh_Phdr *p);
int		elfsh_segment_is_executable(elfsh_Phdr *p);
int		elfsh_set_segment_flags(elfsh_Phdr *p, elfsh_Addr flags);
int		elfsh_set_segment_align(elfsh_Phdr *p, elfsh_Addr align);
int		elfsh_set_segment_memsz(elfsh_Phdr *p, elfsh_Addr memsz);
int		elfsh_set_segment_filesz(elfsh_Phdr *p, elfsh_Addr filesz);
int		elfsh_set_segment_paddr(elfsh_Phdr *p, elfsh_Addr paddr);
int		elfsh_set_segment_vaddr(elfsh_Phdr *p, elfsh_Addr vaddr);
int		elfsh_set_segment_type(elfsh_Phdr *p, elfsh_Addr type);
int		elfsh_set_segment_offset(elfsh_Phdr *p, elfsh_Addr offset);
elfsh_Word      elfsh_get_segment_flags(elfsh_Phdr *p);
elfsh_Word      elfsh_get_segment_align(elfsh_Phdr *p);
elfsh_Word      elfsh_get_segment_memsz(elfsh_Phdr *p);
elfsh_Word      elfsh_get_segment_filesz(elfsh_Phdr *p);
elfsh_Addr      elfsh_get_segment_paddr(elfsh_Phdr *p);
elfsh_Addr      elfsh_get_segment_vaddr(elfsh_Phdr *p);
elfsh_Word	elfsh_get_segment_type(elfsh_Phdr *p);
elfsh_Off	elfsh_get_segment_offset(elfsh_Phdr *p);
elfsh_Phdr	*elfsh_get_pht_entry_by_index(elfsh_Phdr *pht, elfsh_Addr index);
elfsh_Addr	elfsh_get_object_baseaddr(elfshobj_t *file);
void		elfsh_endianize_pht(elfsh_Phdr *pht, char byteorder, uint16_t sz);
elfsh_Phdr	elfsh_create_phdr(elfsh_Word t, elfsh_Addr a, elfsh_Off z, elfsh_Word al);
elfsh_Phdr	*elfsh_insert_phdr(elfshobj_t *file, elfsh_Phdr *h);
int		elfsh_remove_phdr(elfshobj_t *current, int index);

void		*elfsh_get_rpht(elfshobj_t *file, int *num);
int		elfsh_create_rpht(elfshobj_t *file);
elfsh_Phdr	*elfsh_insert_runtime_phdr(elfshobj_t *file, elfsh_Phdr *h);

/* hash.c */
int           elfsh_get_hashchain_num(int *c);
int           elfsh_set_hashchain_num(int *c, int v);
int           elfsh_get_hashbucket_num(int *b);
int           elfsh_set_hashbucket_num(int *b, int v);
int           elfsh_get_hashchain_value(int *c);
int           elfsh_set_hashchain_value(int *c, int v);
int           elfsh_get_hashbucket_value(int *b);
int           elfsh_set_hashbucket_value(int *b, int v);
int           *elfsh_get_hashchain_by_name(elfshobj_t *f, char *s);
int           *elfsh_get_hashchain_by_index(void *p, int i);
int           *elfsh_get_hashbucket_by_name(elfshobj_t *f, char *s);
int           *elfsh_get_hashbucket_by_index(void *p, int i);
int           *elfsh_get_hashchain(const void *d);
int           elfsh_get_hashnchain(const void *d);
int           *elfsh_get_hashbucket(const void *d);
int           elfsh_get_hashnbucket(const void *d);
void          *elfsh_get_hashtable(elfshobj_t *file, int *n);
int	      elfsh_get_symbol_hash(char *sym_name);
int	      elfsh_get_dynsymbol_by_hash(elfshobj_t *file, char *sym_name);

/* got.c */
elfsh_Addr     	*elfsh_get_got(elfshobj_t *file, int *num);
elfshsect_t     *elfsh_get_got_by_idx(elfshobj_t *file, elfsh_Addr range, u_int *nbr);
elfsh_Addr     	*elfsh_get_got_entry_by_index(elfsh_Addr *got, elfsh_Addr index);
elfsh_Addr     	*elfsh_get_got_entry_by_name(elfshobj_t *file, char *name);
int		elfsh_set_got_entry_by_index(elfshobj_t *file, int index, elfsh_Addr a);
int		elfsh_set_got_entry_by_name(elfshobj_t *file, char *name, elfsh_Addr a);
int		elfsh_set_got_entry(elfsh_Addr *got, elfsh_Addr vaddr);
elfsh_Addr     	elfsh_get_got_entry(elfsh_Addr *got);
elfsh_Addr      elfsh_get_got_val(elfsh_Addr *got);
u_int		elfsh_set_got_val(elfsh_Addr *got, elfsh_Addr val);
elfsh_Addr      elfsh_get_got_addr(elfsh_Addr *got);
u_int		elfsh_set_got_addr(elfsh_Addr *got, elfsh_Addr val);
int     	elfsh_get_got_symbol_reloc(elfshobj_t *file, uint8 *name,
                                 elfsh_Rel *rel_entry);
int		elfsh_endianize_got(elfshsect_t *_new);
elfshsect_t	*elfsh_get_gotsct(elfshobj_t *file);
int		elfsh_shift_got(elfshobj_t *file, u_int size);
int		elfsh_shift_altgot(elfshobj_t *file, u_int size);

/* dtors.c */
elfsh_Addr     	*elfsh_get_dtors(elfshobj_t *file, int *num);
elfsh_Addr     	*elfsh_get_dtors_entry_by_index(elfsh_Addr *dtors, elfsh_Addr index);
elfsh_Addr     	*elfsh_get_dtors_entry_by_name(elfshobj_t *file, char *name);
elfsh_Addr     	elfsh_get_dtors_entry(elfsh_Addr *dtors);
int		elfsh_set_dtors_entry_by_index(elfshobj_t *file, int index, elfsh_Addr a);
int		elfsh_set_dtors_entry_by_name(elfshobj_t *file, char *name, elfsh_Addr a);
int		elfsh_set_dtors_entry(elfsh_Addr *dtors, elfsh_Addr vaddr);
int		elfsh_shift_dtors(elfshobj_t *file, u_int size);

/* ctors.c */
elfsh_Addr     	*elfsh_get_ctors(elfshobj_t *file, int *num);
elfsh_Addr     	*elfsh_get_ctors_entry_by_index(elfsh_Addr *ctors, elfsh_Addr index);
elfsh_Addr     	*elfsh_get_ctors_entry_by_name(elfshobj_t *file, char *name);
elfsh_Addr     	elfsh_get_ctors_entry(elfsh_Addr *ctors);
int		elfsh_set_ctors_entry_by_name(elfshobj_t *file, char *name, elfsh_Addr a);
int		elfsh_set_ctors_entry_by_index(elfshobj_t *file, int index, elfsh_Addr a);
int		elfsh_set_ctors_entry(elfsh_Addr *ctors, elfsh_Addr vaddr);
int		elfsh_shift_ctors(elfshobj_t *file, u_int size);

/* section.c */
elfshsect_t	*elfsh_get_section_by_type(elfshobj_t *, u_int type, int range, int *, int *, int *);
elfshsect_t	*elfsh_get_section_by_index(elfshobj_t *, elfsh_Addr index, int *, int *);
elfshsect_t	*elfsh_get_section_by_name(elfshobj_t *, char *name, int *, int *, int *);
elfshsect_t	*elfsh_get_parent_section(elfshobj_t *file, elfsh_Addr addr, elfsh_SAddr *offset);
elfshsect_t	*elfsh_get_parent_section_by_foffset(elfshobj_t *file, u_int foff, elfsh_SAddr *offset);
elfshsect_t	*elfsh_create_section(char *name);
elfshsect_t	*elfsh_get_section_from_sym(elfshobj_t *file, elfsh_Sym *sym);
void		*elfsh_load_section(elfshobj_t *file, elfsh_Shdr *shdr);
void		*elfsh_get_anonymous_section(elfshobj_t *file, elfshsect_t *sect);
int		elfsh_append_data_to_section(elfshsect_t *sect, void *input, u_int len);
char		elfsh_shift_section(elfshsect_t *sct, elfshsect_t *tmp, u_char mode);
int		elfsh_fill_section(elfshsect_t *sect, char c, u_int size);
int		elfsh_remove_section(elfshobj_t *obj, char *name);
elfshsect_t	*elfsh_get_section_list(elfshobj_t *file, int *num);
elfshsect_t	*elfsh_get_tail_section(elfshobj_t *file);
elfshsect_t	*elfsh_get_section_by_idx(elfshsect_t *list, elfsh_Addr index);
void		*elfsh_get_section_data(elfshsect_t *obj, u_int off, u_int sizelem);
int		elfsh_add_section(elfshobj_t *file, elfshsect_t *sect, u_int index, void *data, int shift);
int             elfsh_add_runtime_section(elfshobj_t *file, elfshsect_t *sct, u_int range, void *dat);

int		elfsh_write_section_data(elfshsect_t *sect, u_int off, char *data, u_int size, u_int sizelem);
elfshsect_t	*elfsh_get_section_by_nam(elfshobj_t *file, char *name);
void		*elfsh_get_raw(elfshsect_t *sect);
int		elfsh_section_is_runtime(elfshsect_t *sect);
elfshsect_t     *elfsh_get_rsection_by_index(elfshobj_t *file, elfsh_Addr idx, int *stridx, int *num);



/* inject.c */
elfshsect_t	*elfsh_insert_section(elfshobj_t *file, char *name, char *data, char mode, u_int size, u_int mod);
int		elfsh_insert_unmapped_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data);
int		elfsh_insert_mapped_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data, int mode, u_int modulo);
int		elfsh_insert_data_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data);
int		elfsh_insert_code_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data, u_int mod);
int		elfsh_insert_code_section_up(elfshobj_t*, elfshsect_t*, elfsh_Shdr, void*, u_int);
int		elfsh_insert_section_idx(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data, u_int index);
int		elfsh_insert_static_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data, int mode, u_int mod);
int		elfsh_insert_runtime_section(elfshobj_t *file, elfshsect_t *sect, elfsh_Shdr hdr, void *data, int mode, u_int mod);


/* version.c */
elfsh_Word    elfsh_get_verneed_aux(elfsh_Verneed *n);
int           elfsh_set_verneed_aux(elfsh_Verneed *n, elfsh_Word v);
elfsh_Half    elfsh_get_verneed_cnt(elfsh_Verneed *n);
int           elfsh_set_verneed_cnt(elfsh_Verneed *n, elfsh_Half v);
elfsh_Word    elfsh_get_verneed_file(elfsh_Verneed *n);
int           elfsh_set_verneed_file(elfsh_Verneed *n, elfsh_Word v);
elfsh_Word    elfsh_get_verneed_next(void *p);
int           elfsh_set_verneed_next(void *p, elfsh_Word v);
elfsh_Half    elfsh_get_verneed_flags(elfsh_Vernaux *n);
int           elfsh_set_verneed_flags(elfsh_Vernaux *n, elfsh_Half v);
elfsh_Word    elfsh_get_verneed_hash(elfsh_Vernaux *n);
int           elfsh_set_verneed_hash(elfsh_Vernaux *n, elfsh_Word v);
elfsh_Word    elfsh_get_verneed_name(elfsh_Vernaux *n);
int           elfsh_set_verneed_name(elfsh_Vernaux *n, elfsh_Word v);
elfsh_Half    elfsh_get_verneed_ndx(elfsh_Vernaux *n);
int           elfsh_set_verneed_ndx(elfsh_Vernaux *n, elfsh_Half v);
elfsh_Verneed *elfsh_get_verneed_by_name(elfshobj_t *f, char *n);
elfsh_Vernaux *elfsh_get_verneed_entry_by_index(void *p, elfsh_Half i);
elfsh_Word    elfsh_get_verdef_cname(elfsh_Verdaux *d);
int           elfsh_set_verdef_cname(elfsh_Verdaux *d, elfsh_Word v);
elfsh_Word    elfsh_get_verdef_next(void *d);
int           elfsh_set_verdef_next(void *d, elfsh_Word v);
elfsh_Word    elfsh_get_verdef_aux(elfsh_Verdef *d);
int           elfsh_set_verdef_aux(elfsh_Verdef *d, elfsh_Word v);
elfsh_Half    elfsh_get_verdef_flags(elfsh_Verdef *d);
int           elfsh_set_verdef_flags(elfsh_Verdef *d, elfsh_Half v);
elfsh_Word    elfsh_get_verdef_hash(elfsh_Verdef *d);
int           elfsh_set_verdef_hash(elfsh_Verdef *d, elfsh_Word v);
elfsh_Half    elfsh_get_verdef_cnt(elfsh_Verdef *d);
int           elfsh_set_verdef_cnt(elfsh_Verdef *d, elfsh_Half v);
elfsh_Half    elfsh_get_verdef_ndx(elfsh_Verdef *d);
int           elfsh_set_verdef_ndx(elfsh_Verdef *d, elfsh_Half v);
void          *elfsh_get_verdef_entry_by_index(void *p, elfsh_Half i);
elfsh_Half    *elfsh_get_versym_entry_by_index(elfsh_Half *s, u_int i);
elfsh_Half    elfsh_get_versym_val(elfsh_Half *s);
int           elfsh_set_versym_val(elfsh_Half *s, elfsh_Half v);
elfsh_Half    *elfsh_get_versym_by_name(elfshobj_t *f, char *n);
int           elfsh_get_verdauxnamelist(elfshobj_t *f, hashdef_t *hdef, char **n, int nu);
int           elfsh_load_needtable(hash_t *t, void *p, u_int s);
int           elfsh_load_deftable(hash_t *t, void *p, u_int s);
void          *elfsh_get_versymtab(elfshobj_t *f, int *n);
void          *elfsh_get_verneedtab(elfshobj_t *f, int *n);
void          *elfsh_get_verdeftab(elfshobj_t *f, int *n);
elfshsect_t   *elfsh_get_verdeftab_by_idx(elfshobj_t *f, elfsh_Addr r, int *n);
void          *elfsh_get_verstrtable(elfshobj_t *f);
char          *elfsh_get_verneedfile(elfshobj_t *f, elfsh_Verneed *n);
char          *elfsh_get_vernauxname(elfshobj_t *f, elfsh_Vernaux *a);
char          *elfsh_get_verdauxname(elfshobj_t *f, elfsh_Verdaux *a);

/* reloc.c */
elfshsect_t	*elfsh_get_reloc(elfshobj_t *file, elfsh_Addr range, u_int *num);
void		elfsh_free_reloc_list(elfshobj_t *file);
u_int		elfsh_set_reltype(elfsh_Rel *r, elfsh_Addr type);
u_int		elfsh_set_relsym(elfsh_Rel *r, elfsh_Addr sym);
int             elfsh_set_reloffset(elfsh_Rel *r, elfsh_Addr off);
u_int		elfsh_get_reltype(elfsh_Rel *r);
u_int		elfsh_get_relsym(elfsh_Rel *r);
elfsh_Addr      elfsh_get_reloffset(elfsh_Rel *r);
void		elfsh_setrel(char b);	/* scripting flag for dynamic 'Rel vs Rela' info */
elfsh_Rel	*elfsh_get_relent_by_index(elfsh_Rel *table, elfsh_Addr index);
elfshrel_t	*elfsh_find_rel(elfshsect_t *sect);
int		elfsh_insert_relent(elfshsect_t *sect, elfsh_Rel *rel);
char		*elfsh_get_symname_from_reloc(elfshobj_t *file, elfsh_Rel *r);
elfsh_Sym	*elfsh_get_symbol_from_reloc(elfshobj_t *file, elfsh_Rel *r);
elfsh_Rel	*elfsh_get_relent_by_name(elfshobj_t *file, char *name);
elfsh_Sword     elfsh_get_reladdend(elfsh_Rela *r);
int             elfsh_set_reladdend(elfsh_Rela *r, elfsh_Addr val);
int		elfsh_endianize_relocs(elfshsect_t *s);
elfsh_Rel	elfsh_create_relent(elfsh_Addr type, elfsh_Addr sym, 
				    elfsh_Addr off);
elfsh_Rela	elfsh_create_relaent(elfsh_Addr type, elfsh_Addr sym, 
				     elfsh_Addr off, elfsh_Addr add);

/* interp.c */
char		*elfsh_get_interp(elfshobj_t *file);
int             elfsh_write_interp(elfshobj_t *file, char *interp);

/* notes */
elfshsect_t	*elfsh_get_notes(elfshobj_t *file, elfsh_Addr range);

void		elfsh_free_notes_list(elfshobj_t *file);

/* plt.c */
elfshsect_t	*elfsh_get_plt(elfshobj_t *file, int *num);
int		elfsh_is_pltentry(elfshobj_t *file, elfsh_Sym *sym);
int		elfsh_is_plt(elfshobj_t *file, elfshsect_t *s);
int		elfsh_is_altplt(elfshobj_t *file, elfshsect_t *s);
int		elfsh_get_pltentsz(elfshobj_t *file);
int		elfsh_get_first_pltentsz(elfshobj_t *file);

/* altplt.c */
int		elfsh_relink_plt(elfshobj_t *file, u_int modulo);
int		elfsh_copy_plt(elfshobj_t *file, u_int modulo);
int		elfsh_get_plt_symbol_offset(elfshobj_t *file, uint8 *name,
					    elfsh_Off *off);
int		elfsh_build_plt(elfshobj_t *file);
int		elfsh_altplt_firstent(elfshsect_t	*new, 
				      u_int		*off,
				      elfshsect_t	*symtab, 
				      elfshobj_t	*file, 
				      elfshsect_t	*plt,
				      elfshsect_t	*extplt,
				      uint32_t		diff);

/* altgot.c */
int		elfsh_redirect_pltgot(elfshobj_t *file, elfshsect_t *altgot, elfshsect_t *got, 
				      elfshsect_t *plt, elfshsect_t *altplt);
int		elfsh_shift_generic_relocs(elfshobj_t *file, elfsh_Addr diff, 
					   elfshsect_t *relplt);
int		elfsh_shift_alpha_relocs(elfshobj_t *file, char *name, 
					 elfshsect_t *altgot, u_int off);
int		elfsh_shift_ia32_relocs(elfshobj_t *file, elfsh_Addr  diff,
					elfshsect_t *relplt, elfsh_Addr limit);
void		elfsh_shift_mips_relocs(elfshobj_t *file, elfsh_Addr diff);
int		elfsh_shift_sparc_relocs(elfshobj_t *file, elfsh_Addr diff, 
					 elfshsect_t *relplt);
int		elfsh_reencode_pltentry(elfshobj_t *file, elfshsect_t *plt, 
					uint32_t diff, u_int off);
int		elfsh_reencode_first_pltentry(elfshobj_t *file, elfshsect_t *plt, 
					      uint32_t diff);

/* extplt.c */
int		elfsh_extplt_mirror_sections(elfshobj_t *file);
elfsh_Sym	*elfsh_request_pltent(elfshobj_t *file, char *name);
	

/* raw.c */
int		elfsh_raw_write(elfshobj_t *file, u_int dst, void *src, int len);
int		elfsh_raw_read(elfshobj_t *file, u_int dst, void *src, int len);
int		elfsh_get_foffset_from_vaddr(elfshobj_t *file, elfsh_Addr vaddr);
int		elfsh_get_vaddr_from_foffset(elfshobj_t *file, u_int foffset);

/* remap.c */
int             elfsh_reloc_pht(elfshobj_t *file, elfsh_Addr diff);
int             elfsh_reloc_sht(elfshobj_t *file, elfsh_Addr diff);
int             elfsh_relocate_section(elfshsect_t *sect, elfsh_Addr diff);

int             elfsh_reloc_raw(elfshsect_t *cur, elfsh_Addr diff);
int             elfsh_reloc_symtab(elfshsect_t *s, elfsh_Addr diff);
int             elfsh_reloc_rel(elfshsect_t *sect, elfsh_Addr diff);
int             elfsh_reloc_got(elfshsect_t *sect, elfsh_Addr diff);
int             elfsh_reloc_ctors(elfshsect_t *sect, elfsh_Addr diff);
int             elfsh_reloc_dtors(elfshsect_t *sect, elfsh_Addr diff);
int             elfsh_reloc_hash(elfshsect_t *sect, elfsh_Addr diff);
int             elfsh_reloc_array(elfshobj_t *file, elfsh_Addr *array, u_int size, elfsh_Addr diff);
int             elfsh_relocate(elfshobj_t *file);

/* map.c */
int		elfsh_read_obj(elfshobj_t *file);
elfshobj_t	*elfsh_map_obj(char *name);
elfshsect_t *   elfsh_fixup_sctndx(elfshsect_t *symtab);

/* save.c */
int		elfsh_save_obj(elfshobj_t *file, char *name);

/* copy.c */
elfshobj_t	*elfsh_copy_obj(elfshobj_t *file);

/* strtab.c */
int		elfsh_insert_in_strtab(elfshobj_t *file, char *name);
int		elfsh_insert_in_shstrtab(elfshobj_t *file, char *name);
int		elfsh_insert_in_dynstr(elfshobj_t *file, char *name);
elfshsect_t	*elfsh_get_strtab(elfshobj_t *file, int index);
elfshsect_t	*elfsh_rebuild_strtab(elfshobj_t *file);
int             elfsh_insert_in_rshstrtab(elfshobj_t *file, char *name);

/* fixup.c */
elfshsect_t	*elfsh_fixup_symtab(elfshobj_t *file, int *strindex);
int		 elfsh_fixup_dynsymtab(elfshsect_t *dynsym);
elfsh_Sym	 *elfsh_restore_dynsym(elfshobj_t *file, elfshsect_t *plt, u_int off, 
				       elfshsect_t *dynsym);


/* sanitize.c */
void		elfsh_update_nameidx(elfshobj_t *file, int offset, int len);
void		elfsh_update_linkidx(elfshobj_t *file, int low_index, int diff);
void		elfsh_update_linkidx_equ(elfshobj_t *file, int idx, int diff);
void		elfsh_update_symlinkidx_equ(elfshobj_t *file, int idx, int diff);
void		elfsh_update_shidx(elfshobj_t *file, elfshsect_t *tab, int idx, int diff);
void		elfsh_update_symtab_shidx(elfshobj_t *file, int idx, int diff);
void		elfsh_update_dynsym_shidx(elfshobj_t *file, int idx, int diff);


/* sht_rebuild.c */
int		elfsh_rebuild_sht(elfshobj_t *file);
int		elfsh_merge_shtentry(elfshobj_t *file, 
				     int	phdr_index, 
				     elfsh_Shdr shdr, 
				     char	*name);

/* comment.c */
elfshsect_t	*elfsh_get_comments(elfshobj_t *file);
char		*elfsh_get_comments_entry(elfshobj_t *file, u_int range);

/* hijack.c */
int		elfsh_hijack_function_by_name(elfshobj_t	*file,
					      uint32_t		type,
					      char		*name,
					      elfsh_Addr	addr,
					      elfsh_Addr	*hooked);

/* debug.c */
int		elfsh_print_sectlist(elfshobj_t *obj, char *label);

/* bss.c */
int		elfsh_fixup_bss_real(elfshobj_t *file, elfshsect_t *bss, char fixflag);		
elfshsect_t	*elfsh_fixup_bss(elfshobj_t *file);
elfshsect_t	*elfsh_add_bss(elfshobj_t *file);
int		elfsh_flush_bss(elfshobj_t *file);
int		elfsh_cleanup_bss(elfshobj_t *file, elfsh_Phdr *pht);
int		elfsh_find_bsslen(elfshobj_t *host, elfshobj_t *rel, char *name);
elfshsect_t	*elfsh_insert_runtime_bss(elfshobj_t *file, elfshobj_t *rel);

/* relinject.c */
int		elfsh_inject_etrel(elfshobj_t *file, elfshobj_t *rel);
int		elfsh_relocate_object(elfshobj_t *file, elfshobj_t *rel, u_char stage);
int		elfsh_inject_etrel_hash(elfshobj_t *host, elfshobj_t *rel, 
					hash_t *first, hash_t *second);


/* search.c */
int		elfsh_register_working_objects(hash_t *prvhash,hash_t *sharedhash);
elfshobj_t	*elfsh_find_obj_by_symbol(char *name);
elfsh_Sym	*elfsh_strongest_symbol(elfsh_Sym *choice, elfsh_Sym *candidate);

/* sort.c */
int		elfsh_sync_sorted_symtab(elfshsect_t *sect);
int		elfsh_sort_symtab(elfsh_Sym *symtab, int size, int type);

/* hooks.c */
u_char		elfsh_get_ostype(elfshobj_t *file);
u_char		elfsh_get_hosttype(elfshobj_t *file);
u_char		elfsh_get_elftype(elfshobj_t *file);
u_char		elfsh_get_archtype(elfshobj_t *file);
int		elfsh_default_plthandler(elfshobj_t *n, elfsh_Sym *n2, elfsh_Addr n3);
int		elfsh_default_relhandler(elfshsect_t *n, elfsh_Rel *n2, elfsh_Addr *n3, elfsh_Addr n4, elfshsect_t *n5);
int		elfsh_default_cflowhandler(elfshobj_t *n, char *n1, elfsh_Sym *n2, elfsh_Addr n3);
int		elfsh_default_argchandler(elfsh_Addr addr);

int		elfsh_register_altplthook(u_char arch, u_char obj, u_char os, void *fct);
int		elfsh_register_plthook(u_char arch, u_char o, u_char os, void *fct);
int		elfsh_register_relhook(u_char a, u_char o, u_char os, void *fct);
int		elfsh_register_cflowhook(u_char a, u_char o, u_char os, void *fct);
int		elfsh_register_extplthook(u_char a, u_char o, u_char os, void *f);
int		elfsh_register_breakhook(u_char a, u_char o, u_char os, void *fct);
int		elfsh_register_argchook(u_char a, u_char o, u_char os, void *fct);

int             elfsh_register_vector(char      *name,
                                      void      *registerfunc,
                                      void      *defaultfunc,
                                      u_int     *dimensions,
                                      u_int     dimsz);

int		elfsh_get_pagesize(elfshobj_t *file);
u_int		elfsh_get_breaksize(elfshobj_t *file);
void		elfsh_setup_hooks();

int             elfsh_plt(elfshobj_t *file, elfsh_Sym *s, elfsh_Addr new);
int             elfsh_altplt(elfshobj_t *file, elfsh_Sym *s, elfsh_Addr new);
int             elfsh_cflow(elfshobj_t *file, char *name, elfsh_Sym *old,
			    elfsh_Addr new);
int             elfsh_rel(elfshobj_t *file, elfshsect_t *s, elfsh_Rel *r,
			  elfsh_Addr *l, elfsh_Addr a, elfshsect_t *m);
int		elfsh_setbreak(elfshobj_t *file, elfshbp_t *bp);
int             elfsh_encodeplt(elfshobj_t *file, elfshsect_t *plt, 
				elfsh_Addr diff, u_int off);
int             elfsh_encodeplt1(elfshobj_t *file, elfshsect_t *plt, 
				 elfshsect_t *extplt, elfsh_Addr diff);
int             elfsh_extplt(elfshsect_t *extplt, elfshsect_t *altgot, 
			     elfshsect_t *dynsym, elfshsect_t *relplt);
int		*elfsh_args_count(elfshobj_t *file, u_int off, elfsh_Addr vaddr);



/* sparc32.c */
int		elfsh_cflow_sparc32(elfshobj_t  *null,
				    char	*snull,
				    elfsh_Sym*	null2,
				    elfsh_Addr	null3);
int		elfsh_hijack_plt_sparc32(elfshobj_t *file,
					 elfsh_Sym *symbol,
					 elfsh_Addr addr);
int		elfsh_hijack_altplt_sparc32(elfshobj_t *file,
					    elfsh_Sym *symbol,
					    elfsh_Addr addr);
int		elfsh_relocate_sparc32(elfshsect_t	*_new,
				       elfsh_Rela	*cur,
				       elfsh_Addr	*dword,
				       elfsh_Addr       addr,
				       elfshsect_t	*mod);

/* sparc64.c */
int		elfsh_cflow_sparc64(elfshobj_t  *null,
				    char	*snull,
				    elfsh_Sym	*null2,
				    elfsh_Addr	null3);
int		elfsh_hijack_plt_sparc64(elfshobj_t *file,
				       elfsh_Sym *symbol,
				       elfsh_Addr addr);
int		elfsh_hijack_altplt_sparc64(elfshobj_t *file,
					  elfsh_Sym *symbol,
					  elfsh_Addr addr);
int		elfsh_relocate_sparc64(elfshsect_t	*_new,
				       elfsh_Rela      	*cur,
				       elfsh_Addr      	*dword,
				       elfsh_Addr       addr,
				       elfshsect_t      *mod);

/* ia64.c */
int		elfsh_cflow_ia64(elfshobj_t     *null,
				 char		*sname,
				 elfsh_Sym	*null2,
				 elfsh_Addr	null3);
int		elfsh_hijack_plt_ia64(elfshobj_t *file,
				      elfsh_Sym *symbol,
				      elfsh_Addr addr);
int		elfsh_hijack_altplt_ia64(elfshobj_t *file,
					 elfsh_Sym *symbol,
					 elfsh_Addr addr);
int		elfsh_relocate_ia64(elfshsect_t	*_new,
				    elfsh_Rela   *cur,
				    elfsh_Addr   *dword,
				    elfsh_Addr   addr,
				    elfshsect_t  *mod);

/* alpha64.c */
int		elfsh_cflow_alpha64(elfshobj_t  *null,
				    char	*sname,
				    elfsh_Sym	*null2,
				    elfsh_Addr	null3);
int		elfsh_hijack_plt_alpha64(elfshobj_t *file,
					 elfsh_Sym *symbol,
					 elfsh_Addr addr);
int		elfsh_hijack_altplt_alpha64(elfshobj_t *file,
					    elfsh_Sym *symbol,
					    elfsh_Addr addr);
int		elfsh_relocate_alpha64(elfshsect_t  *_new,
				       elfsh_Rela   *cur,
				       elfsh_Addr   *dword,
				       elfsh_Addr   addr,
				       elfshsect_t  *mod);

/* mips32.c */
int		elfsh_cflow_mips32(elfshobj_t   *null,
				   char		*sname,
				   elfsh_Sym	*null2,
				   elfsh_Addr	null3);
int		elfsh_hijack_plt_mips32(elfshobj_t *file,
					elfsh_Sym *symbol,
					elfsh_Addr addr);
int		elfsh_hijack_altplt_mips32(elfshobj_t *file,
					   elfsh_Sym *symbol,
					   elfsh_Addr addr);
int		elfsh_relocate_mips32(elfshsect_t  *_new,
				      elfsh_Rel    *cur,
				      elfsh_Addr   *dword,
				      elfsh_Addr   addr,
				      elfshsect_t  *mod);

/* mips64.c */
int		elfsh_cflow_mips64(elfshobj_t   *null,
				   char		*sname,
				   elfsh_Sym	*null2,
				   elfsh_Addr	null3);
int		elfsh_hijack_plt_mips64(elfshobj_t *file,
					elfsh_Sym *symbol,
					elfsh_Addr addr);
int		elfsh_hijack_altplt_mips64(elfshobj_t *file,
					   elfsh_Sym *symbol,
					   elfsh_Addr addr);
int		elfsh_relocate_mips64(elfshsect_t  *_new,
				      elfsh_Rel    *cur,
				      elfsh_Addr   *dword,
				      elfsh_Addr   addr,
				      elfshsect_t  *mod);


/* ia32.c */
int		elfsh_reencode_pltentry_ia32(elfshobj_t   *file, 
					     elfshsect_t  *plt, 
					     uint32_t     diff, 
					     u_int	     off);
int		elfsh_reencode_first_pltentry_ia32(elfshobj_t  *file, 
						   elfshsect_t *plt, 
						   uint32_t	diff);

int		elfsh_encodeplt1_ia32(elfshobj_t *file, 
				      elfshsect_t *plt, 
				      elfshsect_t *extplt,
				      elfsh_Addr diff);
int		elfsh_encodeplt_ia32(elfshobj_t *file, 
				     elfshsect_t *plt, 
				     elfsh_Addr diff,
				     u_int	off);
void		*elfsh_getret_ia32(void *frame);
void		*elfsh_bt_ia32(void *frame);

int		elfsh_extplt_ia32(elfshsect_t *extplt, elfshsect_t *altgot,
				  elfshsect_t *dynsym, elfshsect_t *relplt);
int		elfsh_break_ia32(elfshobj_t *f,
				 elfshbp_t  *bp);

int		elfsh_cflow_ia32(elfshobj_t      *null,
				 char		*sname,
				 elfsh_Sym	*null2,
				 elfsh_Addr	null3);
int		elfsh_hijack_plt_ia32(elfshobj_t *file,
				      elfsh_Sym *symbol,
				      elfsh_Addr addr);
int		elfsh_hijack_plt_ia32_etdyn(elfshobj_t *file,
					    elfsh_Sym *symbol,
					    elfsh_Addr addr);
int		elfsh_relocate_ia32(elfshsect_t	*_new,
				    elfsh_Rel	*cur,
				    elfsh_Addr	*dword,
				    elfsh_Addr	addr,
				    elfshsect_t *mod);
int           *elfsh_args_count_ia32(elfshobj_t *file, u_int off, elfsh_Addr vaddr);

/* reginfo.c */
elfsh_Sword	*elfsh_get_gpvalue_addr(elfshobj_t* file);
int             elfsh_set_gpvalue(elfshobj_t* file, elfsh_Addr gp);
elfsh_Sword     elfsh_get_gpvalue(elfshobj_t* file);


/* runtime.c */
elfsh_Addr	 elfsh_runtime_map(elfsh_Phdr *segment);
int		elfsh_runtime_unmap(elfsh_Phdr *segment);
int		elfsh_set_phdr_prot(u_int mode);
int		elfsh_munprotect(elfshobj_t *obj, elfsh_Addr addr, uint32_t sz);
int		elfsh_mprotect(elfsh_Addr addr, uint32_t sz, int prot);

/* error.c */
void		elfsh_profile_reset(u_int sel);
int		elfsh_profile_print(char *file, char *func, u_int line, char *msg);
void		elfsh_profile_err(char *file, char *func, u_int line, char *msg);
void		elfsh_profile_out(char *file, char *func, u_int line);
void		elfsh_error();
void		elfsh_incdepth();
void		elfsh_decdepth();
void		elfsh_updir();

/* state.c */
u_char		elfsh_is_static_mode(); 
void		elfsh_set_static_mode();
u_char		elfsh_is_debug_mode();
void		elfsh_set_debug_mode();
void		elfsh_set_mode(u_char mode);
u_char		elfsh_get_mode();
void		elfsh_toggle_mode();
int		elfsh_is_prof_enable();
int		elfsh_prof_disable();
int		elfsh_prof_enable_err();
int		elfsh_prof_enable_out();
u_char		elfsh_debugger_present();
void		elfsh_set_profile(int (*profile)(char *), int (*profile_err)(char *));
void		elfsh_set_color_simple(void (*endline)(), char *(*colorinstr)(char *text),
				       char *(*colorstr)(char *t), char *(*colorfieldstr)(char *t),
				       char *(*colortypestr)(char *t), char *(*colorend)(char *text),
				       char *(*colorwarn)(char *text));
void		elfsh_set_color_advanced(char *(*coloradv)(char *ty, char *p, char *te),
					 char *(*colorinstr_fmt)(char* p, char *t),
					 char *(*coloraddress)(char *p, elfsh_Addr a),
					 char *(*colornumber)(char *p, u_int n),
					 char *(*colorstr_fmt)(char *p, char *t),
					 char *(*colorfieldstr_fmt)(char *p, char *t),
					 char *(*colortypestr_fmt)(char *p, char *t),
					 char *(*colorwarn_fmt)(char *pattern, char *text));

/* linkmap.c */
elfsh_Addr	elfsh_linkmap_get_laddr(elfshlinkmap_t *lm);
void		elfsh_linkmap_set_laddr(elfshlinkmap_t *lm, elfsh_Addr laddr);
char		*elfsh_linkmap_get_lname(elfshlinkmap_t *lm);
void		elfsh_linkmap_set_lname(elfshlinkmap_t *lm, char *name);
elfsh_Dyn	*elfsh_linkmap_get_lld(elfshlinkmap_t *lm);
void		elfsh_linkmap_set_lld(elfshlinkmap_t *lm, elfsh_Dyn *lld);
elfshlinkmap_t	*elfsh_linkmap_get_lnext(elfshlinkmap_t *lm);
void		elfsh_linkmap_set_lnext(elfshlinkmap_t *lm, elfshlinkmap_t *lnext);
elfshlinkmap_t	*elfsh_linkmap_get_lprev(elfshlinkmap_t *lm);
void		elfsh_linkmap_set_lprev(elfshlinkmap_t *lm, elfshlinkmap_t *lprev);
elfshlinkmap_t	*elfsh_linkmap_by_name(elfshlinkmap_t *lm, char *name);

/* bt.c */
void		*elfsh_bt(void *frame);
void		*elfsh_bt_get_frame(ucontext_t *);

/* bp.c */
int		elfsh_bp_add(hash_t *bps, elfshobj_t *file, 
			     char *resolv, elfsh_Addr addr, 
			     u_char flags);

#endif /* __LIBELFSH_H_ */
