/*
** libelfsh-compat.h for libelfsh
** 
** Started on  Tue Feb 18 04:15:42 2003 mayhem
** Last update Thu Jun 26 04:37:24 2003 mayhem
*/

#undef NULL
#define NULL 0

#if defined(__linux__)
 #include <endian.h>
#elif defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)
 #include <machine/endian.h>
#elif defined(sun)
 #define __LITTLE_ENDIAN 1234
 #define __BIG_ENDIAN 4321
 #define __BYTE_ORDER __BIG_ENDIAN
#endif

#ifndef swap32
#define swap32(x)							\
	((unsigned long)(						\
	(((unsigned long)(x) & (unsigned long) 0x000000ffUL) << 24) |	\
	(((unsigned long)(x) & (unsigned long) 0x0000ff00UL) <<  8) |	\
	(((unsigned long)(x) & (unsigned long) 0x00ff0000UL) >>  8) |	\
	(((unsigned long)(x) & (unsigned long) 0xff000000UL) >> 24) ))
#endif

#ifndef swap16
#define swap16(x)							\
	((unsigned short)(						\
	(((unsigned short)(x) & (unsigned short) 0x00ffU) << 8) |	\
	(((unsigned short)(x) & (unsigned short) 0xff00U) >> 8) ))
#endif


/* Portability Stuffs */
#if defined(__OpenBSD__) || defined(__NetBSD__)
 #include <sys/exec_elf.h>
#elif defined(__linux__) || defined(__FreeBSD__) || defined(sun)
 #include <elf.h>
#endif

/* SPARC relocation types, undefined on FreeBSD */
#ifndef R_SPARC_NONE
 #define R_SPARC_NONE 0
#endif
#ifndef R_SPARC_8
 #define R_SPARC_8       1              
#endif
#ifndef R_SPARC_16
 #define R_SPARC_16      2              
#endif
#ifndef R_SPARC_32
 #define R_SPARC_32      3              
#endif
#ifndef R_SPARC_DISP8
 #define R_SPARC_DISP8   4              
#endif
#ifndef R_SPARC_DISP16
 #define R_SPARC_DISP16  5            
#endif
#ifndef R_SPARC_DISP32
 #define R_SPARC_DISP32  6               
#endif
#ifndef R_SPARC_WDISP30
 #define R_SPARC_WDISP30 7               
#endif
#ifndef R_SPARC_WDISP22
 #define R_SPARC_WDISP22 8               
#endif
#ifndef R_SPARC_HI22
 #define R_SPARC_HI22    9              
#endif
#ifndef R_SPARC_22
 #define R_SPARC_22      10            
#endif
#ifndef R_SPARC_13
 #define R_SPARC_13      11         
#endif
#ifndef R_SPARC_LO10
 #define R_SPARC_LO10    12             
#endif
#ifndef R_SPARC_GOT10
 #define R_SPARC_GOT10   13             
#endif 
#ifndef R_SPARC_GOT13
 #define R_SPARC_GOT13   14             
#endif
#ifndef R_SPARC_GOT22
 #define R_SPARC_GOT22   15              
#endif
#ifndef R_SPARC_PC10
 #define R_SPARC_PC10    16              
#endif
#ifndef R_SPARC_PC22
 #define R_SPARC_PC22    17             
#endif
#ifndef R_SPARC_WPLT30
 #define R_SPARC_WPLT30  18            
#endif
#ifndef R_SPARC_COPY
 #define R_SPARC_COPY    19           
#endif
#ifndef R_SPARC_GLOB_DAT
 #define R_SPARC_GLOB_DAT 20         
#endif
#ifndef R_SPARC_JMP_SLOT
 #define R_SPARC_JMP_SLOT 21 
#endif
#ifndef R_SPARC_RELATIVE
 #define R_SPARC_RELATIVE 22   
#endif
#ifndef R_SPARC_UA32
 #define R_SPARC_UA32    23      
#endif

/* Advanced .dynamic entries : Undefined on BSD */
#ifndef O_SYNC			
 #define O_SYNC O_FSYNC
#endif
#ifndef DT_BIND_NOW	
 #define DT_BIND_NOW	24
#endif
#ifndef DT_SYMINENT
 #define DT_SYMINIENT	0x6ffffdff
#endif
#ifndef DT_FLAGS
 #define DT_FLAGS       30
#endif
#ifndef DT_VERDEFNUM
 #define DT_VERDEFNUM	0x6ffffffd
#endif
#ifndef DT_VERNEEDNUM
 #define DT_VERNEEDNUM	0x6fffffff
#endif
#ifndef DT_PLTADSZ
 #define DT_PLTPADSZ	0x6ffffdf9
#endif
#ifndef DT_VERDEF
 #define DT_VERDEF	0x6ffffffc
#endif
#ifndef DT_SYMINENT
 #define DT_SYMINENT	0x6ffffdff
#endif
#ifndef DT_RELACOUNT
 #define DT_RELACOUNT	0x6ffffff9
#endif
#ifndef DT_RELCOUNT
 #define DT_RELCOUNT	0x6ffffffa
#endif
#ifndef DT_SYMINFO
 #define DT_SYMINFO	0x6ffffeff
#endif
#ifndef DT_SYMINSZ
 #define DT_SYMINSZ	0x6ffffdfe
#endif
#ifndef DT_CHECKSUM
 #define DT_CHECKSUM	0x6ffffdf8
#endif
#ifndef DT_VERSYM
 #define DT_VERSYM	0x6ffffff0
#endif
#ifndef DT_MOVESZ
 #define DT_MOVESZ	0x6ffffdfb
#endif
#ifndef DT_VERNEED
 #define DT_VERNEED	0x6ffffffe
#endif
#ifndef DT_FILTER
 #define DT_FILTER	0x7fffffff
#endif
#ifndef DT_MOVEENT
 #define DT_MOVEENT	0x6ffffdfa
#endif
#ifndef DT_AUXILIARY
 #define DT_AUXILIARY	0x7ffffffd
#endif
#ifndef DT_INIT_ARRAY
 #define DT_INIT_ARRAY	25
#endif 
#ifndef DT_FINI_ARRAY
 #define DT_FINI_ARRAY	26
#endif
#ifndef DT_INIT_ARRAYSZ
 #define DT_INIT_ARRAYSZ	27
#endif
#ifndef DT_FINI_ARRAYSZ
 #define DT_FINI_ARRAYSZ	28
#endif
#ifndef DT_FLAGS_1
 #define DT_FLAGS_1		0x6ffffffb
#endif
#ifndef DT_FEATURE_1
 #define DT_FEATURE_1		0x6ffffdfc
#endif
#ifndef DT_POSFLAG_1
 #define DT_POSFLAG_1		0x6ffffdfd
#endif

#ifndef ELFMAG
 #define ELFMAG		"\177ELF"
#endif


#ifndef DF_P1_LAZYLOAD
 #define DF_P1_LAZYLOAD	0x00000001	/* Lazyload following object.  */
#endif

#ifndef DF_P1_GROUPPERM
 #define DF_P1_GROUPPERM	0x00000002	/* Symbols from next object are not */
#endif

#ifndef DF_ORIGIN
 #define DF_ORIGIN	0x00000001	/* Object may use df_origin */
#endif

#ifndef DF_SYMBOLIC
 #define DF_SYMBOLIC	0x00000002	/* Symbol resolutions starts here */
#endif

#ifndef DF_TEXTREL
 #define DF_TEXTREL	0x00000004	/* Object contains text relocations */
#endif

#ifndef DF_BIND_NOW
 #define DF_BIND_NOW	0x00000008	/* No lazy binding for this object */
#endif

#ifndef DF_1_NOW
 #define DF_1_NOW	0x00000001	/* Set RTLD_NOW for this object.  */
#endif

#ifndef DF_1_GLOBAL
 #define DF_1_GLOBAL	0x00000002	/* Set RTLD_GLOBAL for this object.  */
#endif

#ifndef DF_1_GROUP
 #define DF_1_GROUP	0x00000004	/* Set RTLD_GROUP for this object.  */
#endif

#ifndef DF_1_NODELETE
 #define DF_1_NODELETE	0x00000008	/* Set RTLD_NODELETE for this object.*/
#endif

#ifndef DF_1_LOADFLTR
 #define DF_1_LOADFLTR	0x00000010	/* Trigger filtee loading at runtime.*/
#endif

#ifndef DF_1_INITFIRST
 #define DF_1_INITFIRST	0x00000020	/* Set RTLD_INITFIRST for this object*/
#endif

#ifndef DF_1_NOOPEN
 #define DF_1_NOOPEN	0x00000040	/* Set RTLD_NOOPEN for this object.  */
#endif

#ifndef DF_1_ORIGIN
 #define DF_1_ORIGIN	0x00000080	/* $ORIGIN must be handled.  */
#endif

#ifndef DF_1_DIRECT
 #define DF_1_DIRECT	0x00000100	/* Direct binding enabled.  */
#endif

#ifndef DF_1_TRANS
 #define DF_1_TRANS	0x00000200
#endif

#ifndef DF_1_INTERPOSE
 #define DF_1_INTERPOSE	0x00000400	/* Object is used to interpose.  */
#endif

#ifndef DF_1_NODEFLIB
 #define DF_1_NODEFLIB	0x00000800	/* Ignore default lib search path.  */
#endif

#ifndef DF_1_NODUMP
 #define DF_1_NODUMP	0x00001000
#endif

#ifndef DF_1_CONFALT
 #define DF_1_CONFALT	0x00002000
#endif

#ifndef DF_1_ENDFILTEE
 #define DF_1_ENDFILTEE	0x00004000
#endif

 
#ifndef DT_RUNPATH
 #define DT_RUNPATH	29		/* Library search path */
#endif

#ifndef DT_ENCODING
 #define DT_ENCODING	32		/* Start of encoded range */
#endif

#ifndef DT_PREINIT_ARRAY
 #define DT_PREINIT_ARRAY 32		/* Array with addresses of preinit fct*/
#endif

#ifndef DT_PREINIT_ARRAYSZ
 #define DT_PREINIT_ARRAYSZ 33		/* size in bytes of DT_PREINIT_ARRAY */
#endif


#ifndef	DT_NUM
 #define	DT_NUM		34		/* Number used */
#endif

#ifndef DTF_1_PARINIT
 #define DTF_1_PARINIT	0x00000001
#endif

#ifndef DTF_1_CONFEXP
 #define DTF_1_CONFEXP	0x00000002
#endif


#ifndef STT_COMMON		/* undefined on debian */
 #define STT_COMMON	5
#endif

/* Legal values for sh_flags (section flags) undefined on (Free)BSD */

#ifndef SHF_WRITE
 #define SHF_WRITE            (1 << 0)   /* Writable */
#endif

#ifndef SHF_ALLOC
 #define SHF_ALLOC            (1 << 1)   /* Occupies memory during execution */
#endif

#ifndef SHF_EXECINSTR
 #define SHF_EXECINSTR        (1 << 2)   /* Executable */
#endif

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

#ifndef SHF_OS_NONCONFORMING
 #define SHF_OS_NONCONFORMING (1 << 8)   /* Non-standard OS specific handling */
#endif

#ifndef SHF_MASKOS
 #define SHF_MASKOS           0x0ff00000 /* OS-specific.  */
#endif

#ifndef SHF_MASKPROC
 #define SHF_MASKPROC         0xf0000000 /* Processor-specific */
#endif

#ifndef ELF32_ST_VISIBILITY
 #define ELF32_ST_VISIBILITY(o)	((o) & 0x03)
#endif


#if defined(sun)		/* SunOS has its own include */
 #include <strings.h>
 #include "libelfsh-sunos.h"
#endif

#if defined(ELFSH_INTERN)
 #include "libelfsh-internal.h"
#endif

