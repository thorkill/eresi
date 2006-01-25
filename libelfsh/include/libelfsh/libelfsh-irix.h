/*
** libelfsh-irix.h
**
** Various undefined macros on IRIX
**
** Last update Mon Aug 23 05:05:27 2005 ym
*/




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

