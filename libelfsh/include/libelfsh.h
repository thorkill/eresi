/*
** libelfsh.h for libelfsh
**
** Started on  Mon Jul 23 15:47:12 2001 mayhem
** Last update Sat Aug 16 22:27:30 2003 jv
*/

#ifndef __LIBELFSH_H_
 #define __LIBELFSH_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <libelfsh-compat.h>

/* Configure the DEBUG modes for various part of the code */
#define		__DEBUG_MAP__		0
#define		__DEBUG_SECTS__		0
#define		__DEBUG_BLOCKS__	0
#define		__DEBUG_SORT__		0
#define		__DEBUG_RELADD__	0
#define		__DEBUG_COPYPLT__	0
#define		__DEBUG_BSS__		0


/* libelfsh block hijack types */
/* lower byte is OS, middle byte is CPU type and the highest is hijack type */

#define		ELFSH_HIJACK_CPU_MASK		0x0000ff
#define		ELFSH_HIJACK_CPU_ANY		EM_NONE
#define		ELFSH_HIJACK_CPU_i86		EM_386
#define		ELFSH_HIJACK_CPU_SPARC		EM_SPARC

#define		ELFSH_HIJACK_OS_MASK		0x00ff00
#define		ELFSH_HIJACK_OS_ANY		0x000000
#define		ELFSH_HIJACK_OS_LINUX		0x000100
#define		ELFSH_HIJACK_OS_SOLARIS		0x000200

#define		ELFSH_HIJACK_TYPE_MASK		0xff0000
#define		ELFSH_HIJACK_TYPE_NONE		0x000000
#define		ELFSH_HIJACK_TYPE_GOT		0x010000
#define		ELFSH_HIJACK_TYPE_PLT		0x020000

/* Some defined values */
#define		ELFSH_SECTION_NAME_MAPPED	".mapped"
#define		ELFSH_SECTION_NAME_UNMAPPED	".unmapped"
#define		ELFSH_SECTION_NAME_EXTRAREL	".elfsh.rel"
#define		ELFSH_SECTION_NAME_ALTPLT	".orig.plt"

/* We need this for string based research, and for sht reconstruction */
#define		ELFSH_SECTION_NAME_PLT		".plt"
#define		ELFSH_SECTION_NAME_DTORS	".dtors"
#define		ELFSH_SECTION_NAME_CTORS	".ctors"
#define		ELFSH_SECTION_NAME_GOT		".got"
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


/* Section index in the secthash */
#define		ELFSH_SECTION_NULL		0
#define		ELFSH_SECTION_INTERP		1
#define		ELFSH_SECTION_NOTE		2
#define		ELFSH_SECTION_HASH		3
#define		ELFSH_SECTION_DYNSYM		4
#define		ELFSH_SECTION_DYNSTR		5
#define		ELFSH_SECTION_GNUVER		6
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
#define		ELFSH_SECTION_UNKNOWN		255
#define		ELFSH_SECTION_MAX		256

/* Some constants */
#define		ELFSH_PLT_ENTRY_SIZE	        16
#define		ELFSH_SYMTAB_ENTRY_SIZE		sizeof(Elf32_Sym)
#define		ELFSH_REL_ENTRY_SIZE		sizeof(Elf32_Rel)

#define		ELFSH_STARTSYM			"_start"
#define		ELFSH_NULL_STRING		"(NULL)"
#define		ELFSH_SORT_BY_ADDR		'a'
#define		ELFSH_SORT_BY_SIZE		's'

/* Dedicated to you know who */
#define		ELFSH_MEANING			42

#define		ELFSH_SECTION_FIRST		0
#define		ELFSH_SECTION_LAST		0xFFFF
#define		ELFSH_BLOCK_FIRST		0
#define		ELFSH_BLOCK_LAST		0xFFFF

#define		ELFSH_SHIFTING_ABSENT		0
#define		ELFSH_SHIFTING_NONE		1
#define		ELFSH_SHIFTING_PARTIAL		2
#define		ELFSH_SHIFTING_COMPLETE		3

#define		ELFSH_UNSET_OFFSET		(1 << 30)

#define		ELFSH_LOWSYM			1
#define		ELFSH_HIGHSYM			2
#define		ELFSH_EXACTSYM			3

#define		ELFSH_DATA_INJECTION		0
#define		ELFSH_CODE_INJECTION		1

#define		ELFSH_PAGESIZE			(getpagesize())
#define		STT_BLOCK			6


#define	     DUMPABLE(sym)	(elfsh_get_symbol_type(sym) == STT_FUNC   || \
				 elfsh_get_symbol_type(sym) == STT_OBJECT || \
				 elfsh_get_symbol_type(sym) == STT_COMMON || \
				 elfsh_get_symbol_type(sym) == STT_SECTION)



#define FILE_IS_SPARC(obj) (((elfsh_get_arch((obj)->hdr) == EM_SPARC) || \
                           (elfsh_get_arch((obj)->hdr) == EM_SPARC32PLUS) || \
                           (elfsh_get_arch((obj)->hdr) == EM_SPARCV9)) ? 1 : 0)


#define IS_REL(scn)		(FILE_IS_SPARC((scn)->parent) ? 0 : 1)





/* Pax flags (to be read in elfhdr.e_flags) */
#define		ELFSH_PAX_PAGEEXEC         1    /* Paging based non-exec pages */
#define		ELFSH_PAX_EMULTRAMP        2    /* Emulate trampolines */
#define		ELFSH_PAX_MPROTECT         4    /* Restrict mprotect() */
#define		ELFSH_PAX_RANDMMAP         8    /* Randomize mmap() base */
#define		ELFSH_PAX_RANDEXEC         16   /* Randomize ET_EXEC base */
#define	        ELFSH_PAX_SEGMEXEC         32   /* Segmentation based non-exec pages */
#define		EI_PAX			   14   /* Index in e_ident[] where to read flags */


/* ELFsh private stab entry */
typedef struct		s_stab
{
  unsigned long		strindex;
  unsigned char		type;
  unsigned char		other;
  unsigned short	desc;
  unsigned int		value;
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


/* Block type */
typedef struct		s_block
{
  Elf32_Sym		*sym;
  struct s_sect		*section;	/* section the block belongs to	*/
  int			offset;		/* Section offset for this block	*/
  int			len;		/* block real lenght			*/
  struct s_block	*next;
}			elfshblock_t;


/* Also use the same format for PLT entries */
typedef elfshblock_t	elfshpltent_t;



/* BSS Zone type */
typedef struct	s_zone
{
  char		*modname;		/* Relocatable object name */
  u_int		off;			/* Offset from the start of the section for this zone */
  u_int		size;			/* Zone size */
  struct s_zone	*next;			/* Next zone */
}		elfshzone_t;



/* Section data type */
typedef struct		s_sect
{

  /* Filled at creation */
  char			*name;		/* Cached name							*/
  struct s_obj		*parent;	/* Parent Object						*/
  Elf32_Phdr		*phdr;		/* the first parent segment					*/

  /* Filled at creation, can be modified when SHT is sorted */
  Elf32_Shdr		*shdr;		/* section header for this section				*/
  int			index;		/* Section index in sht						*/
  struct s_sect		*next;		/* Next section in the list					*/
  struct s_sect		*prev;		/* Prev section in the list					*/

  /* ELFsh section state */
#define		ELFSH_SECTION_ANALYZED		(1 << 0)
#define		ELFSH_SECTION_INSERTED		(1 << 1)
#define		ELFSH_SECTION_MOVED		(1 << 2)
  char			flags;		/* Analysed/Raw, Orphelin/Inserted, Moved/Notmoved	        */

  /* Filled at loading */
  void			*data;		/* Section's data cache						*/

  /* Filled at analysing */
  void			*altdata;	/* Type dependant internal format				*/
  void			*terdata;	/* Alternate type dependant internal format			*/

  /* Filled at relocation */
  elfshrel_t		*rel;		/* ELFsh private relocation table for this section		*/
  u_int			srcref;		/* Number of absolute reference FROM this section : sizeof(rel)	*/
  u_int			dstref;		/* Number of absolute reference TO this section			*/

  /* Filled at merging (BSS only for the moment) */
  elfshzone_t		*modzone;	/* Extra zone list, one zone per object */

}			elfshsect_t;


/* ELF object structure */
typedef struct	s_obj
{
  Elf32_Ehdr	*hdr;				/* Elf header */
  Elf32_Shdr	*sht;				/* Section header table */
  Elf32_Phdr	*pht;				/* Program Header table */
  elfshsect_t	*sectlist;			/* Section linked list */
  elfshsect_t	*secthash[ELFSH_SECTION_MAX];	/* Section hash table */

  int		fd;			/* File descriptor for the original file */
  char		*name;			/* Object path */
  int		type;			/* ELFSH_OBJECT_CORE, ELFSH_OBJECT_SHARED, ELFSH_OBJECT_RELOC or ELFSH_OBJECT_EXEC */
  int		rights;			/* 0 = RO, 1 = WR */
  time_t	loadtime;		/* Time of Loading */
  u_int		id;			/* Object ID */
  char		scanned;		/* Has the object already been block scanned ? */
  char		read;			/* Has the object already been read ? */
  char		shtrm;			/* Mark SHT and Unmapped sections as stripped ? */
  char		strip;			/* Mark file as stripped */
  char		shtrb;			/* Reconstruct the SHT if non present */
  char		buff[ELFSH_MEANING];	/* Internal buffer, sometimes used to avoid a malloc */
  const char	*error;			/* Last error string */
  struct s_obj	*next;			/* The list is simply linked */
}		elfshobj_t;





/*
**
** Libelfsh API prototypes
**
** Check elfsh/doc/libelfsh-api.txt for a complete description
**
** XXX: libelfsh-api.txt not updated to 0.5 (it is for 0.43b)
**
*/


/* dynamic.c */
Elf32_Dyn	*elfsh_get_dynamic(elfshobj_t *file, int *num);
Elf32_Dyn	*elfsh_get_dynamic_entry_by_type(elfshobj_t *file, char type);
Elf32_Sword	elfsh_get_dynentry_type(Elf32_Dyn *d);
int		elfsh_set_dynentry_type(Elf32_Dyn *d, Elf32_Sword tag);
Elf32_Word	elfsh_get_dynentry_val(Elf32_Dyn *d);
int		elfsh_set_dynentry_val(Elf32_Dyn *d, Elf32_Word val);
Elf32_Dyn	*elfsh_get_dynamic_entry_by_index(Elf32_Dyn *dynamic, u_int index);
char		*elfsh_get_dynentry_string(elfshobj_t *file, Elf32_Dyn *ent);

/* symbol.c */
Elf32_Sym	*elfsh_get_symbol_by_name(elfshobj_t *file, char *name);
char		*elfsh_reverse_symbol(elfshobj_t *file, u_int sym_value, int *offset);
char		*elfsh_get_symbol_name(elfshobj_t *file, Elf32_Sym *s);
void		*elfsh_get_symtab(elfshobj_t *file, int *num);
Elf32_Sym	*elfsh_get_symbol_by_value(elfshobj_t *file, u_int vaddr, int *off, int mode);
int		elfsh_strip(elfshobj_t *file);
int		elfsh_set_symbol_name(elfshobj_t *file, Elf32_Sym *s, char *name);
int		elfsh_shift_symtab(elfshobj_t *file, u_long lim, int inc);
int		elfsh_insert_sectsym(elfshobj_t *file, elfshsect_t *sect);
int		elfsh_get_symbol_foffset(elfshobj_t *file, Elf32_Sym *sym);

/* dynsym.c */
Elf32_Sym	*elfsh_get_dynsymbol_by_name(elfshobj_t *file, char *name);
void		*elfsh_get_dynsymtab(elfshobj_t *file, int *num);
char		*elfsh_reverse_dynsymbol(elfshobj_t *file, u_int sym_value, int *offset);
char		*elfsh_get_dynsymbol_name(elfshobj_t *file, Elf32_Sym *s);
int		elfsh_set_dynsymbol_name(elfshobj_t *file, Elf32_Sym *s, char *name);

/* sym_common.c */
Elf32_Sym	elfsh_create_symbol(int value, int size, int type, int binding, int vis, int idx);
Elf32_Sym	*elfsh_copy_symtab(void *addr, int size);
Elf32_Sym	*elfsh_merge_symtabs(Elf32_Sym *one, Elf32_Sym *two, int size_one, int size_two);
Elf32_Sym	*elfsh_get_sym_by_value(Elf32_Sym *sym, int num, u_int vaddr, int *off, int mode);
int		elfsh_insert_symbol(elfshsect_t *sect, Elf32_Sym *sym, char *name);
Elf32_Word      elfsh_get_symbol_size(Elf32_Sym *s);
int		elfsh_set_symbol_size(Elf32_Sym *s, Elf32_Word size);
Elf32_Addr      elfsh_get_symbol_value(Elf32_Sym *s);
int		elfsh_set_symbol_value(Elf32_Sym *s, Elf32_Addr value);
u_char		elfsh_get_symbol_bind(Elf32_Sym *s);
u_char		elfsh_set_symbol_bind(Elf32_Sym *s, char opt);
u_char		elfsh_get_symbol_type(Elf32_Sym *s);
u_char		elfsh_set_symbol_type(Elf32_Sym *s, char type);
u_char		elfsh_get_symbol_visibility(Elf32_Sym *s);
u_int		elfsh_set_symbol_visibility(Elf32_Sym *s, char vis);
u_int		elfsh_get_symbol_link(Elf32_Sym *s);
u_int		elfsh_set_symbol_link(Elf32_Sym *s, u_int val);

Elf32_Sym	*elfsh_get_symbol_by_index(Elf32_Sym *symtab, u_int index);
Elf32_Sym	*elfsh_get_metasym_by_name(elfshobj_t *file, char *name);
char		*elfsh_reverse_metasym(elfshobj_t *file, u_int vaddr, int *off);

/* obj.c */
elfshobj_t	*elfsh_load_obj(char *name);
void		elfsh_unload_obj(elfshobj_t *file);

/* elf.c */
int		elfsh_load_hdr(elfshobj_t *file);
void		*elfsh_get_hdr(elfshobj_t *file);
Elf32_Off       elfsh_get_shtoff(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_shtnbr(Elf32_Ehdr *e);
Elf32_Off       elfsh_get_phtoff(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_phtnbr(Elf32_Ehdr *e);
Elf32_Addr      elfsh_get_entrypoint(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_arch(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_objtype(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_shstrtab_index(Elf32_Ehdr *e);
Elf32_Word      elfsh_get_version(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_ehsize(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_phentsize(Elf32_Ehdr *e);
Elf32_Half      elfsh_get_shentsize(Elf32_Ehdr *e);
int             elfsh_set_shtoff(Elf32_Ehdr *e, Elf32_Off off);
int             elfsh_set_shtnbr(Elf32_Ehdr *e, Elf32_Half num);
int             elfsh_set_phtoff(Elf32_Ehdr *e, Elf32_Off num);
int		elfsh_set_phtnbr(Elf32_Ehdr *e, Elf32_Half num);
int             elfsh_set_entrypoint(Elf32_Ehdr *e, Elf32_Addr addr);
u_int		elfsh_set_arch(Elf32_Ehdr *e, Elf32_Half val);
u_int		elfsh_set_objtype(Elf32_Ehdr *e, Elf32_Half val);
u_int		elfsh_set_shstrtab_index(Elf32_Ehdr *e, Elf32_Half val);
u_int		elfsh_set_version(Elf32_Ehdr *e, Elf32_Word val);
u_int		elfsh_set_ehsize(Elf32_Ehdr *e, Elf32_Half val);
u_int		elfsh_set_phentsize(Elf32_Ehdr *e, Elf32_Half val);
u_int		elfsh_set_shentsize(Elf32_Ehdr *e, Elf32_Half val);
int		elfsh_get_encoding(Elf32_Ehdr *hdr);
int		elfsh_set_encoding(Elf32_Ehdr *hdr, u_char type);
int		elfsh_set_flags(Elf32_Ehdr *hdr, Elf32_Word flags);
Elf32_Word	elfsh_get_flags(Elf32_Ehdr *hdr);
int		elfsh_set_magic(Elf32_Ehdr *hdr, u_int mag);
int		elfsh_get_magic(Elf32_Ehdr *hdr);
int		elfsh_set_class(Elf32_Ehdr *hdr, u_char class);
int		elfsh_get_class(Elf32_Ehdr *hdr);

/* pax.c */
int		elfsh_set_paxflags(Elf32_Ehdr *hdr, Elf32_Word flags);
Elf32_Word	elfsh_get_paxflags(Elf32_Ehdr *hdr);
char            elfsh_set_pax_pageexec(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_pageexec(Elf32_Ehdr *hdr);
char            elfsh_set_pax_emultramp(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_emultramp(Elf32_Ehdr *hdr);
char            elfsh_set_pax_mprotect(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_mprotect(Elf32_Ehdr *hdr);
char            elfsh_set_pax_randmmap(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_randmmap(Elf32_Ehdr *hdr);
char            elfsh_set_pax_randexec(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_randexec(Elf32_Ehdr *hdr);
char            elfsh_set_pax_segmexec(Elf32_Ehdr *hdr, u_short off);
char            elfsh_get_pax_segmexec(Elf32_Ehdr *hdr);


/* stab.c */
void		*elfsh_get_stab(elfshobj_t *file, int *num);
void		*elfsh_get_stab_offset(elfshstabent_t *s);
char		*elfsh_get_stab_name(elfshobj_t *file, elfshstabent_t *s);
u_int		elfsh_get_stab_type(elfshstabent_t *s);


/* sht.c */
Elf32_Shdr      *elfsh_get_shtentry_from_sym(elfshobj_t *file, Elf32_Sym *sym);
void		*elfsh_get_sht(elfshobj_t *file, int *num);
int		elfsh_load_sht(elfshobj_t *file);
Elf32_Sym	*elfsh_get_sym_from_shtentry(elfshobj_t *file, Elf32_Shdr *hdr);
Elf32_Word      elfsh_get_section_info(Elf32_Shdr *s);
Elf32_Word      elfsh_get_section_entsize(Elf32_Shdr *s);
Elf32_Word      elfsh_get_section_link(Elf32_Shdr *s);
Elf32_Off       elfsh_get_section_foffset(Elf32_Shdr *s);
Elf32_Addr      elfsh_get_section_addr(Elf32_Shdr *s);
Elf32_Word      elfsh_get_section_align(Elf32_Shdr *s);
Elf32_Word      elfsh_get_section_size(Elf32_Shdr *s);
Elf32_Word      elfsh_get_section_type(Elf32_Shdr *s);
int             elfsh_set_section_info(Elf32_Shdr *s, Elf32_Word info);
int             elfsh_set_section_entsize(Elf32_Shdr *s, Elf32_Word entsize);
int             elfsh_set_section_link(Elf32_Shdr *s, Elf32_Word link);
int		elfsh_set_section_foffset(Elf32_Shdr *s, Elf32_Off offset);
int		elfsh_set_section_addr(Elf32_Shdr *s, Elf32_Addr addr);
int		elfsh_set_section_align(Elf32_Shdr *s, Elf32_Word align);
int		elfsh_set_section_size(Elf32_Shdr *s, Elf32_Word size);
int		elfsh_set_section_type(Elf32_Shdr *s, Elf32_Word type);
char		elfsh_get_section_execflag(Elf32_Shdr *s);
char		elfsh_get_section_writableflag(Elf32_Shdr *s);
char		elfsh_get_section_allocflag(Elf32_Shdr *s);
char		elfsh_get_section_mergeableflag(Elf32_Shdr *s);
char		elfsh_get_section_strflag(Elf32_Shdr *s);
char		elfsh_get_section_linkflag(Elf32_Shdr *s);
char		elfsh_get_section_orderflag(Elf32_Shdr *s);
char		elfsh_set_section_execflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_writableflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_allocflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_mergeableflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_strflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_linkflag(Elf32_Shdr *s, char val);
char		elfsh_set_section_orderflag(Elf32_Shdr *s, char val);
int		elfsh_insert_section_header(elfshobj_t *file, Elf32_Shdr hdr, u_int range, char *name);
void		elfsh_remove_sht(elfshobj_t *file);
int		elfsh_set_section_name(elfshobj_t *file, elfshsect_t *sect, char *name);
char		*elfsh_get_section_name(elfshobj_t *file, elfshsect_t *s);
Elf32_Shdr	elfsh_create_shdr(Elf32_Word name, Elf32_Word type, Elf32_Word flags,
				  Elf32_Addr addr, Elf32_Off offset, Elf32_Word size,
				  Elf32_Word link, Elf32_Word info, Elf32_Word align,
				  Elf32_Word entsize);
int			elfsh_sort_sht(elfshobj_t *file);



Elf32_Shdr	*elfsh_get_sht_entry_by_index(Elf32_Shdr *s, u_int index);
Elf32_Shdr	*elfsh_get_sht_entry_by_name(elfshobj_t *file, char *name);



/* pht.c */
void		*elfsh_get_pht(elfshobj_t *file, int *num);
Elf32_Phdr	*elfsh_get_segment_by_type(elfshobj_t *file, int type, int range);
Elf32_Phdr	*elfsh_get_parent_segment(elfshobj_t *file, elfshsect_t *new);
int		elfsh_segment_is_parent(elfshsect_t *new, Elf32_Phdr *p);
int		elfsh_load_pht(elfshobj_t *file);
int		elfsh_segment_is_writable(Elf32_Phdr *p);
int		elfsh_segment_is_readable(Elf32_Phdr *p);
int		elfsh_segment_is_executable(Elf32_Phdr *p);
int		elfsh_set_segment_flags(Elf32_Phdr *p, Elf32_Word flags);
int		elfsh_set_segment_align(Elf32_Phdr *p, Elf32_Word align);
int		elfsh_set_segment_memsz(Elf32_Phdr *p, Elf32_Word memsz);
int		elfsh_set_segment_filesz(Elf32_Phdr *p, Elf32_Word filesz);
int		elfsh_set_segment_paddr(Elf32_Phdr *p, Elf32_Addr paddr);
int		elfsh_set_segment_vaddr(Elf32_Phdr *p, Elf32_Addr vaddr);
int		elfsh_set_segment_type(Elf32_Phdr *p, Elf32_Word type);
int		elfsh_set_segment_offset(Elf32_Phdr *p, Elf32_Off offset);
Elf32_Word      elfsh_get_segment_flags(Elf32_Phdr *p);
Elf32_Word      elfsh_get_segment_align(Elf32_Phdr *p);
Elf32_Word      elfsh_get_segment_memsz(Elf32_Phdr *p);
Elf32_Word      elfsh_get_segment_filesz(Elf32_Phdr *p);
Elf32_Addr      elfsh_get_segment_paddr(Elf32_Phdr *p);
Elf32_Addr      elfsh_get_segment_vaddr(Elf32_Phdr *p);
Elf32_Word	elfsh_get_segment_type(Elf32_Phdr *p);
Elf32_Off	elfsh_get_segment_offset(Elf32_Phdr *p);
Elf32_Phdr	*elfsh_get_pht_entry_by_index(Elf32_Phdr *pht, u_int index);
Elf32_Addr	elfsh_get_object_baseaddr(elfshobj_t *file);


/* hash.c */
void		*elfsh_get_hashtable(elfshobj_t *file);
int		elfsh_get_symbol_hash(char *sym_name);
int		elfsh_get_dynsymbol_by_hash(elfshobj_t *file, char *sym_name);

/* got.c */
u_long		*elfsh_get_got(elfshobj_t *file, int *num);
u_long		*elfsh_get_got_entry_by_index(u_long *got, u_int index);
u_long		*elfsh_get_got_entry_by_name(elfshobj_t *file, char *name);
int		elfsh_set_got_entry_by_index(elfshobj_t *file, int index, u_long a);
int		elfsh_set_got_entry_by_name(elfshobj_t *file, char *name, u_long a);
int		elfsh_set_got_entry(u_long *got, u_long vaddr);
int		elfsh_get_got_entry(u_long *got);

/* dtors.c */
u_long		*elfsh_get_dtors(elfshobj_t *file, int *num);
u_long		*elfsh_get_dtors_entry_by_index(u_long *dtors, u_int index);
u_long		*elfsh_get_dtors_entry_by_name(elfshobj_t *file, char *name);
int		elfsh_get_dtors_entry(u_long *dtors);
int		elfsh_set_dtors_entry_by_index(elfshobj_t *file, int index, u_long a);
int		elfsh_set_dtors_entry_by_name(elfshobj_t *file, char *name, u_long a);
int		elfsh_set_dtors_entry(u_long *dtors, u_long vaddr);

/* ctors.c */
u_long		*elfsh_get_ctors(elfshobj_t *file, int *num);
u_long		*elfsh_get_ctors_entry_by_index(u_long *ctors, u_int index);
u_long		*elfsh_get_ctors_entry_by_name(elfshobj_t *file, char *name);
int		elfsh_get_ctors_entry(u_long *ctors);
int		elfsh_set_ctors_entry_by_name(elfshobj_t *file, char *name, u_long a);
int		elfsh_set_ctors_entry_by_index(elfshobj_t *file, int index, u_long a);
int		elfsh_set_ctors_entry(u_long *ctors, u_long vaddr);



/* section.c */
elfshsect_t	*elfsh_get_section_by_type(elfshobj_t *, u_int type, int range, int *, int *, int *);
elfshsect_t	*elfsh_get_section_by_index(elfshobj_t *, int index, int *, int *);
elfshsect_t	*elfsh_get_section_by_name(elfshobj_t *, char *name, int *, int *, int *);
elfshsect_t	*elfsh_get_parent_section(elfshobj_t *file, u_int addr, int *offset);
elfshsect_t	*elfsh_get_parent_section_by_foffset(elfshobj_t *file, u_int foff, int *offset);
elfshsect_t	*elfsh_create_section(char *name);
elfshsect_t	*elfsh_get_section_from_sym(elfshobj_t *file, Elf32_Sym *sym);
void		*elfsh_load_section(elfshobj_t *file, Elf32_Shdr *shdr);
void		*elfsh_get_anonymous_section(elfshobj_t *file, elfshsect_t *sect);
int		elfsh_append_data_to_section(elfshsect_t *sect, void *input, u_int len);
char		elfsh_shift_section(elfshsect_t *sct, elfshsect_t *tmp, u_char mode);

int		elfsh_insert_unmapped_section(elfshobj_t *file, elfshsect_t *sect, Elf32_Shdr hdr, void *data);
int		elfsh_insert_mapped_section(elfshobj_t *file, elfshsect_t *sect, Elf32_Shdr hdr, void *data, int mode);
int		elfsh_insert_data_section(elfshobj_t *file, elfshsect_t	*sect, Elf32_Shdr hdr, void *data);
int		elfsh_insert_code_section(elfshobj_t *file, elfshsect_t *sect, Elf32_Shdr hdr, void *data);
int		elfsh_insert_section(elfshobj_t *file, elfshsect_t *sect, Elf32_Shdr hdr, void *data, u_int index);

elfshsect_t	*elfsh_get_section_list(elfshobj_t *file, int *num);
elfshsect_t	*elfsh_get_tail_section(elfshobj_t *file);

elfshsect_t	*elfsh_get_section_by_idx(elfshsect_t *list, int index);
void		*elfsh_get_section_data(elfshsect_t *obj, u_int off, u_int sizelem);
int		elfsh_add_section(elfshobj_t *file, elfshsect_t *sect, u_int index, void *data, int shift);
int		elfsh_insert_code_section(elfshobj_t *file, elfshsect_t *sect, Elf32_Shdr hdr, void *data);
int		elfsh_write_section_data(elfshsect_t *sect, u_int off, char *data, u_int size, u_int sizelem);
elfshsect_t	*elfsh_get_section_by_nam(elfshobj_t *file, char *name);


/* reloc.c */
elfshsect_t	*elfsh_get_reloc(elfshobj_t *file, u_int range, u_int *num);
void		elfsh_free_reloc_list(elfshobj_t *file);
u_int		elfsh_set_reltype(Elf32_Rel *r, Elf32_Word type);
u_int		elfsh_set_relsym(Elf32_Rel *r, Elf32_Word sym);
int             elfsh_set_reloffset(Elf32_Rel *r, Elf32_Addr off);
u_int		elfsh_get_reltype(Elf32_Rel *r);
u_int		elfsh_get_relsym(Elf32_Rel *r);
Elf32_Addr      elfsh_get_reloffset(Elf32_Rel *r);
Elf32_Rel	*elfsh_get_relent_by_index(Elf32_Rel *table, u_int index);
elfshrel_t	*elfsh_find_rel(elfshsect_t *sect);
int		elfsh_insert_relent(elfshsect_t *sect, Elf32_Rel *rel);
char		*elfsh_get_symname_from_reloc(elfshobj_t *file, Elf32_Rel *r);
Elf32_Sym	*elfsh_get_symbol_from_reloc(elfshobj_t *file, Elf32_Rel *r);
Elf32_Rel	*elfsh_get_relent_by_name(elfshobj_t *file, char *name);
Elf32_Sword     elfsh_get_reladdend(Elf32_Rela *r);
int             elfsh_set_reladdend(Elf32_Rela *r, Elf32_Sword val);


/* interp.c */
char		*elfsh_get_interp(elfshobj_t *file);
int             elfsh_write_interp(elfshobj_t *file, char *interp);

/* notes */
elfshsect_t	*elfsh_get_notes(elfshobj_t *file, u_int range);

void		elfsh_free_notes_list(elfshobj_t *file);

/* blocks.c */
elfshblock_t	*elfsh_get_block_by_addr(elfshobj_t *file, int addr);
elfshblock_t    *elfsh_get_block_by_name(elfshobj_t *file, char *name);
Elf32_Addr	elfsh_get_block_addr(elfshblock_t *func);
u_int		elfsh_get_block_len(elfshblock_t *func);
int		elfsh_insert_block(elfshsect_t *sct, elfshblock_t *fct, int range);
int		elfsh_scan_blocks(elfshobj_t *file);

int		elfsh_print_blocks(elfshsect_t *sect);

/* plt.c */
elfshsect_t	*elfsh_get_plt(elfshobj_t *file, int *num);
int		elfsh_is_pltentry(elfshobj_t *file, Elf32_Sym *sym);
int		elfsh_is_plt(elfshobj_t *file, elfshsect_t *s);
int		elfsh_get_pltentsz(elfshobj_t *file);
int		elfsh_copy_plt(elfshobj_t *file);

/* raw.c */
int		elfsh_raw_write(elfshobj_t *file, u_int dst, void *src, int len);
int		elfsh_raw_read(elfshobj_t *file, u_int dst, void *src, int len);
int		elfsh_get_foffset_from_vaddr(elfshobj_t *file, u_int vaddr);
int		elfsh_get_vaddr_from_foffset(elfshobj_t *file, u_int foffset);

/* remap.c */
int             elfsh_reloc_pht(elfshobj_t *file, u_long diff);
int             elfsh_reloc_sht(elfshobj_t *file, u_long diff);
int             elfsh_relocate_section(elfshsect_t *sect, u_long diff);

int             elfsh_reloc_raw(elfshsect_t *cur, u_long diff);
int             elfsh_reloc_symtab(elfshsect_t *s, u_long diff);
int             elfsh_reloc_rel(elfshsect_t *sect, u_long diff);
int             elfsh_reloc_got(elfshsect_t *sect, u_long diff);
int             elfsh_reloc_ctors(elfshsect_t *sect, u_long diff);
int             elfsh_reloc_dtors(elfshsect_t *sect, u_long diff);
int             elfsh_reloc_hash(elfshsect_t *sect, u_long diff);
int             elfsh_reloc_array(elfshobj_t *file, u_long *array, u_int size, u_long diff);
int             elfsh_relocate(elfshobj_t *file);

/* map.c */
int		elfsh_read_obj(elfshobj_t *file);
elfshobj_t	*elfsh_map_obj(char *name);

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

/* error.c */
void		elfsh_error();

/* fixup.c */
elfshsect_t	*elfsh_fixup_symtab(elfshobj_t *file, int *strindex);

/* sanitize.c */
void		elfsh_update_symlinkidx_equ(elfshobj_t *file, int idx, int diff);
void		elfsh_update_linkidx_equ(elfshobj_t *file, int idx, int diff);
void		elfsh_update_linkidx(elfshobj_t *file, int low_index, int diff);
void		elfsh_update_nameidx(elfshobj_t *file, int offset, int len);

/* sht_rebuild.c */
int		elfsh_rebuild_sht(elfshobj_t *file);

/* comment.c */
elfshsect_t	*elfsh_get_comments(elfshobj_t *file);
char		*elfsh_get_comments_entry(elfshobj_t *file, u_int range);

/* hijack.c */
int		elfsh_hijack_function_by_name(elfshobj_t	*file,
					      uint32_t		type,
					      char		*name,
					      Elf32_Addr	addr);
int		elfsh_hijack_plt_sparc(elfshobj_t *file,
				       Elf32_Sym *symbol,
				       Elf32_Addr addr);
int		elfsh_hijack_plt_i86(elfshobj_t *file,
				     Elf32_Sym *symbol,
				     Elf32_Addr addr);
int		elfsh_hijack_plt_sparc_g2(elfshobj_t *file,
					  Elf32_Sym *symbol,
					  Elf32_Addr addr);

/* debug.c */
int		elfsh_print_sectlist(elfshobj_t *obj, char *label);

/* bss.c */
elfshsect_t	*elfsh_fixup_bss(elfshobj_t *file);
elfshsect_t	*elfsh_add_bss(elfshobj_t *file);
elfshzone_t	*elfsh_create_bsszone(char *name, u_int off, u_int size);
elfshzone_t	*elfsh_find_bsszone(elfshsect_t *bss, char *name);
int		elfsh_add_bsszone(elfshsect_t *bss, elfshzone_t *zone);

/* int		elfsh_find_bsslen(elfshobj_t *host, elfshsect_t *relbss); */
int		elfsh_find_bsslen(elfshobj_t *host, elfshobj_t *rel);

/* relinject.c */
int		elfsh_inject_etrel(elfshobj_t *file, elfshobj_t *rel);

/* sort.c */
int		elfsh_sync_sorted_symtab(elfshsect_t *sect);
int		elfsh_sort_symtab(Elf32_Sym *symtab, int size, int type);


#endif /* __LIBELFSH_H_ */
