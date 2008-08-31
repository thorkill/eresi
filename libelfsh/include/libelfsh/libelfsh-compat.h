/*
** libelfsh-compat.h for libelfsh
** 
** Started on  Tue Feb 18 04:15:42 2003 jfv
** Last update Mon Feb 23 16:23:43 2004 jfv
**
** $Id$
**
*/

#undef NULL
#define NULL 0

/* BSD patch */
#ifndef MAP_ANONYMOUS
 #define MAP_ANONYMOUS  MAP_ANON
#endif

#if defined(__BEOS__)
 #include <elf.h>
#else
 typedef char uint8;
#endif

#include "libelfsh-ia64.h"	/* IA64 defines sometimes not present */
#include "libelfsh-ia32.h"	/* IA32 defines sometimes not present */
#include "libelfsh-alpha.h"	/* Alpha defines sometimes not present */
#include "libelfsh-mips.h"	/* MIPS defines sometimes not present */
#include "libelfsh-sparc.h"	/* SPARC define sometimes not present */
#include "libelfsh-ppc.h"	/* PowerPC define sometimes not present */
#include "libelfsh-cray.h"	/* CRAY define usually not present ;-) */

#if defined(IRIX)
#include "libelfsh-irix.h"	/* IRIX lakes somes recent ELF extension */
#endif

#if defined(sun)		/* SunOS has its own include */
 #include <strings.h>
 #include "libelfsh-sunos.h"
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
 #include "libelfsh-bsd.h"	/* Various BSD need this */
#endif

/* Portability Stuffs */
#ifdef __KERNEL__
 #include "libelfsh-versions.h"
#else
#if defined(__OpenBSD__) || defined(__NetBSD__)
 #include <sys/exec_elf.h>
#elif defined(__linux__) || defined(__FreeBSD__) || defined(sun)
 #include <elf.h>
#endif
#endif


/* Redefinition of BUFSIZ */
#undef  BUFSIZ
#define BUFSIZ 8096

#ifndef EM_ALPHA_EXP
 #define EM_ALPHA_EXP 0x9026
#endif

#ifndef EM_IA_64
 #define EM_IA_64     50
#endif

#ifndef EM_PARISC
 #define EM_PARISC    15
#endif

#ifndef STT_NUM
 #define STT_NUM	7
#endif

#ifndef EM_486
 #define EM_486		6
#endif

#ifndef EM_NCUBE
 #define EM_NCUBE	16
#endif

/* ELFOSABI index in e_ident[] */
#ifndef EI_OSABI
#define	EI_OSABI		7
#endif

/* ELFOSABI types */
#ifndef ELFOSABI_NETBSD
#define	ELFOSABI_NETBSD		2
#endif
#ifndef ELFOSABI_LINUX
#define	ELFOSABI_LINUX		3
#endif
#ifndef ELFOSABI_SOLARIS
#define	ELFOSABI_SOLARIS	6
#endif
#ifndef ELFOSABI_FREEBSD
#define	ELFOSABI_FREEBSD	9
#endif
#ifndef ELFOSABI_OPENBSD
#define	ELFOSABI_OPENBSD	12
#endif

/* Security */
#ifndef PT_GNU_STACK
 #define PT_GNU_STACK    (PT_LOOS + 0x474e551)
#endif
#ifndef PT_GNU_RELRO
 #define PT_GNU_RELRO    (PT_LOOS + 0x474e552)
#endif

#ifndef PT_PAX_FLAGS
 #define PT_PAX_FLAGS    0x65041580
#endif


/* Various printing macros */
#define	XFMT18	"        0x%08X"
#define UFMT14  "    %010u"
#define UFMT18  "        %010u"
#define	UFMT10  "%010u"
#define	DFMT11	"%011d"

/* 64 bits support */
#if defined(ERESI64)
typedef Elf64_Phdr	elfsh_Phdr;
typedef Elf64_Shdr	elfsh_Shdr;
typedef Elf64_Nhdr	elfsh_Nhdr;
typedef Elf64_Sym	elfsh_Sym;
typedef Elf64_Dyn	elfsh_Dyn;
typedef Elf64_Ehdr	elfsh_Ehdr;
typedef Elf64_Word	elfsh_Word;
typedef Elf64_Half	elfsh_Half;
typedef Elf64_Rel	elfsh_Rel;
typedef Elf64_Rela	elfsh_Rela;
typedef Elf64_Sword	elfsh_Sword;
typedef Elf64_Verneed   elfsh_Verneed;
typedef Elf64_Vernaux   elfsh_Vernaux;
typedef Elf64_Verdef    elfsh_Verdef;
typedef Elf64_Verdaux   elfsh_Verdaux;

typedef int64_t		elfsh_SAddr;

#define ELFSH_ST_BIND		ELF64_ST_BIND		
#define ELFSH_ST_TYPE		ELF64_ST_TYPE
#define ELFSH_ST_INFO		ELF64_ST_INFO
#define ELFSH_ST_VISIBILITY	ELF64_ST_VISIBILITY
#define ELFSH_R_SYM		ELF64_R_SYM
#define ELFSH_R_TYPE		ELF64_R_TYPE
#define ELFSH_R_INFO		ELF64_R_INFO
#define ELFSH_M_SYM		ELF64_M_SYM
#define ELFSH_M_SIZE		ELF64_M_SIZE
#define ELFSH_M_INFO		ELF64_M_INFO

#if __WORDSIZE == 32
 #define	XFMT		"0x%016llX"
 #define	AFMT		"%016llX"
 #define	UFMT		"%016llu"
 #define        DFMT		"%016lld"
 #define        RDFMT		"%lld"
#elif __WORDSIZE == 64
 #define	XFMT		"0x%016lX"
 #define	AFMT		"%016lX"
 #define	UFMT		"%016lu"
 #define        DFMT		"%016ld"
 #define        RDFMT		"%ld"
#endif

#if defined(__mips64)
 #define	swaplong(x)	swap32(x)
#else
  #define	swaplong(x)	swap64(x)
#endif

/* 32 bits support */
#elif defined(ERESI32)

typedef Elf32_Phdr	elfsh_Phdr;
typedef Elf32_Shdr	elfsh_Shdr;
typedef Elf32_Nhdr	elfsh_Nhdr;
typedef Elf32_Sym	elfsh_Sym;
typedef Elf32_Dyn	elfsh_Dyn;
typedef Elf32_Ehdr	elfsh_Ehdr;
typedef Elf32_Word	elfsh_Word;
typedef Elf32_Half	elfsh_Half;
typedef Elf32_Rel	elfsh_Rel;
typedef Elf32_Rela	elfsh_Rela;
typedef Elf32_Sword	elfsh_Sword;
typedef Elf32_Verneed   elfsh_Verneed;
typedef Elf32_Vernaux   elfsh_Vernaux;
typedef Elf32_Verdef    elfsh_Verdef;
typedef Elf32_Verdaux   elfsh_Verdaux;

/* We always want to manipulate 32bits in this build */
typedef int32_t		elfsh_SAddr;

#define ELFSH_ST_BIND		ELF32_ST_BIND		
#define ELFSH_ST_TYPE		ELF32_ST_TYPE
#define ELFSH_ST_INFO		ELF32_ST_INFO
#define ELFSH_ST_VISIBILITY	ELF32_ST_VISIBILITY
#define ELFSH_R_SYM		ELF32_R_SYM
#define ELFSH_R_TYPE		ELF32_R_TYPE
#define ELFSH_R_INFO		ELF32_R_INFO
#define ELFSH_M_SYM		ELF32_M_SYM
#define ELFSH_M_SIZE		ELF32_M_SIZE
#define ELFSH_M_INFO		ELF32_M_INFO

#define	XFMT		"0x%08X"
#define	AFMT		"%08X"
#define UFMT		"%08u"
#define DFMT		"%08d"
#define	RDFMT		"%d"

 #define swaplong(x)	swap32(x)

#endif	/* BITS */



/**
 * MIPS reginfo structure
 */
typedef		struct s_reginfo
{
  elfsh_Word	ri_gprmask;
  elfsh_Word	ri_cprmask[4];
  elfsh_Sword	ri_gp_value;
}		elfsh_RegInfo;
