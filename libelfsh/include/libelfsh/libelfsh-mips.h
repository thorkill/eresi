/*
 *
 * Compatibility with the MIPS architecture
 *
 * Define all the relocation types that can be undefined on various OS
 *
 * Last update Sat Feb 26 20:57:27 2004 mayhem
 *
 */


#ifndef EM_MIPS
 #define EM_MIPS 8
#endif

#ifndef EM_MIPS_RS3_LE
 #define EM_MIPS_RS3_LE 10
#endif

/* MIPS relocations.  */
#ifndef R_MIPS_NONE
 #define R_MIPS_NONE             0       /* No reloc */
#endif

#ifndef R_MIPS_16
 #define R_MIPS_16               1       /* Direct 16 bit */
#endif

#ifndef R_MIPS_32
 #define R_MIPS_32               2       /* Direct 32 bit */
#endif

#ifndef R_MIPS_REL32
 #define R_MIPS_REL32            3       /* PC relative 32 bit */
#endif

#ifndef R_MIPS_26
 #define R_MIPS_26               4       /* Direct 26 bit shifted */
#endif

#ifndef R_MIPS_HI16
 #define R_MIPS_HI16             5       /* High 16 bit */
#endif

#ifndef R_MIPS_LO16
 #define R_MIPS_LO16             6       /* Low 16 bit */
#endif

#ifndef R_MIPS_GPREL16
 #define R_MIPS_GPREL16          7       /* GP relative 16 bit */
#endif

#ifndef R_MIPS_LITERAL
 #define R_MIPS_LITERAL          8       /* 16 bit literal entry */
#endif

#ifndef R_MIPS_GOT16
 #define R_MIPS_GOT16            9       /* 16 bit GOT entry */
#endif

#ifndef R_MIPS_PC16
 #define R_MIPS_PC16             10      /* PC relative 16 bit */
#endif

#ifndef R_MIPS_CALL16
 #define R_MIPS_CALL16           11      /* 16 bit GOT entry for function */
#endif

#ifndef R_MIPS_GPREL32
 #define R_MIPS_GPREL32          12      /* GP relative 32 bit */
#endif

#ifndef R_MIPS_SHIFT5
 #define R_MIPS_SHIFT5           16
#endif

#ifndef R_MIPS_SHIFT6
 #define R_MIPS_SHIFT6           17
#endif

#ifndef R_MIPS_64
 #define R_MIPS_64               18
#endif

#ifndef R_MIPS_GOT_DISP
 #define R_MIPS_GOT_DISP         19
#endif

#ifndef R_MIPS_GOT_PAGE
 #define R_MIPS_GOT_PAGE         20
#endif

#ifndef R_MIPS_GOT_OFST
 #define R_MIPS_GOT_OFST         21
#endif

#ifndef R_MIPS_GOT_HI16
 #define R_MIPS_GOT_HI16         22
#endif

#ifndef R_MIPS_GOT_LO16
 #define R_MIPS_GOT_LO16         23
#endif

#ifndef R_MIPS_SUB
 #define R_MIPS_SUB              24
#endif

#ifndef  R_MIPS_INSERT_A
 #define R_MIPS_INSERT_A         25
#endif

#ifndef R_MIPS_INSERT_B
 #define R_MIPS_INSERT_B         26
#endif

#ifndef R_MIPS_DELETE
 #define R_MIPS_DELETE           27
#endif

#ifndef R_MIPS_HIGHER
 #define R_MIPS_HIGHER           28
#endif

#ifndef R_MIPS_HIGHEST
 #define R_MIPS_HIGHEST          29
#endif

#ifndef R_MIPS_CALL_HI16
 #define R_MIPS_CALL_HI16        30
#endif

#ifndef R_MIPS_CALL_LO16
 #define R_MIPS_CALL_LO16        31
#endif

#ifndef R_MIPS_SCN_DISP
 #define R_MIPS_SCN_DISP         32
#endif

#ifndef R_MIPS_REL16
 #define R_MIPS_REL16            33
#endif

#ifndef R_MIPS_ADD_IMMEDIATE
 #define R_MIPS_ADD_IMMEDIATE    34
#endif

#ifndef R_MIPS_PJUMP
 #define R_MIPS_PJUMP            35
#endif

#ifndef R_MIPS_RELGOT
 #define R_MIPS_RELGOT           36
#endif

#ifndef R_MIPS_JALR
 #define R_MIPS_JALR             37
#endif

#ifndef R_MIPS_NUM
 #define R_MIPS_NUM              38
#endif

/* MIPS Dynamic Type */

#ifndef DT_MIPS_RLD_VERSION
#define DT_MIPS_RLD_VERSION	 0x70000001 /* Runtime linker interface version */
#endif

#ifndef DT_MIPS_TIME_STAMP
#define DT_MIPS_TIME_STAMP	 0x70000002 /* Timestamp */
#endif

#ifndef DT_MIPS_ICHECKSUM
#define DT_MIPS_ICHECKSUM	 0x70000003 /* Checksum */
#endif

#ifndef DT_MIPS_IVERSION
#define DT_MIPS_IVERSION	 0x70000004 /* Version string (string tbl index) */
#endif

#ifndef DT_MIPS_FLAGS
#define DT_MIPS_FLAGS		 0x70000005 /* Flags */
#endif

#ifndef DT_MIPS_BASE_ADDRESS
#define DT_MIPS_BASE_ADDRESS	 0x70000006 /* Base address */
#endif

#ifndef DT_MIPS_MSYM
#define DT_MIPS_MSYM		 0x70000007
#endif

#ifndef DT_MIPS_CONFLICT
#define DT_MIPS_CONFLICT	 0x70000008 /* Address of CONFLICT section */
#endif

#ifndef DT_MIPS_LIBLIST
#define DT_MIPS_LIBLIST		 0x70000009 /* Address of LIBLIST section */
#endif

#ifndef DT_MIPS_LOCAL_GOTNO
#define DT_MIPS_LOCAL_GOTNO	 0x7000000a /* Number of local GOT entries */
#endif

#ifndef DT_MIPS_CONFLICTNO
#define DT_MIPS_CONFLICTNO	 0x7000000b /* Number of CONFLICT entries */
#endif

#ifndef DT_MIPS_LIBLISTNO
#define DT_MIPS_LIBLISTNO	 0x70000010 /* Number of LIBLIST entries */
#endif

#ifndef DT_MIPS_SYMTABNO
#define DT_MIPS_SYMTABNO	 0x70000011 /* Number of DYNSYM entries */
#endif

#ifndef DT_MIPS_UNREFEXTNO
#define DT_MIPS_UNREFEXTNO	 0x70000012 /* First external DYNSYM */
#endif

#ifndef DT_MIPS_GOTSYM
#define DT_MIPS_GOTSYM		 0x70000013 /* First GOT entry in DYNSYM */
#endif

#ifndef DT_MIPS_HIPAGENO
#define DT_MIPS_HIPAGENO	 0x70000014 /* Number of GOT page table entries */
#endif

#ifndef DT_MIPS_RLD_MAP
#define DT_MIPS_RLD_MAP		 0x70000016 /* Address of run time loader map.  */
#endif

#ifndef DT_MIPS_DELTA_CLASS
#define DT_MIPS_DELTA_CLASS	 0x70000017 /* Delta C++ class definition.  */
#endif

#ifndef DT_MIPS_DELTA_CLASS_NO
#define DT_MIPS_DELTA_CLASS_NO   0x70000018 /* Number of entries in
                                                DT_MIPS_DELTA_CLASS.  */
#endif

#ifndef DT_MIPS_DELTA_INSTANCE
#define DT_MIPS_DELTA_INSTANCE   0x70000019 /* Delta C++ class instances.  */
#endif

#ifndef DT_MIPS_DELTA_INSTANCE_NO
#define DT_MIPS_DELTA_INSTANCE_NO 0x7000001a /* Number of entries in
                                                DT_MIPS_DELTA_INSTANCE.  */
#endif

#ifndef DT_MIPS_DELTA_RELOC
#define DT_MIPS_DELTA_RELOC	 0x7000001b /* Delta relocations.  */
#endif

#ifndef DT_MIPS_DELTA_RELOC_NO
#define DT_MIPS_DELTA_RELOC_NO	 0x7000001c /* Number of entries in
                                             DT_MIPS_DELTA_RELOC.  */
#endif

#ifndef DT_MIPS_DELTA_SYM
#define DT_MIPS_DELTA_SYM	 0x7000001d /* Delta symbols that Delta
                                           relocations refer to.  */
#endif

#ifndef DT_MIPS_DELTA_SYM_NO
#define DT_MIPS_DELTA_SYM_NO	 0x7000001e /* Number of entries in
                                           DT_MIPS_DELTA_SYM.  */
#endif

#ifndef DT_MIPS_DELTA_CLASSSYM
#define DT_MIPS_DELTA_CLASSSYM	 0x70000020 /* Delta symbols that hold the
                                             class declaration.  */
#endif

#ifndef DT_MIPS_DELTA_CLASSSYM_NO
#define DT_MIPS_DELTA_CLASSSYM_NO 0x70000021 /* Number of entries in
                                                DT_MIPS_DELTA_CLASSSYM.  */
#endif

#ifndef DT_MIPS_CXX_FLAGS
#define DT_MIPS_CXX_FLAGS	 0x70000022 /* Flags indicating for C++ flavor.  */
#endif

#ifndef DT_MIPS_PIXIE_INIT
#define DT_MIPS_PIXIE_INIT	 0x70000023
#endif

#ifndef DT_MIPS_SYMBOL_LIB
#define DT_MIPS_SYMBOL_LIB	 0x70000024
#endif

#ifndef DT_MIPS_LOCALPAGE_GOTIDX
#define DT_MIPS_LOCALPAGE_GOTIDX 0x70000025
#endif

#ifndef DT_MIPS_LOCAL_GOTIDX
#define DT_MIPS_LOCAL_GOTIDX	 0x70000026
#endif

#ifndef DT_MIPS_HIDDEN_GOTIDX
#define DT_MIPS_HIDDEN_GOTIDX	 0x70000027
#endif

#ifndef DT_MIPS_PROTECTED_GOTIDX
#define DT_MIPS_PROTECTED_GOTIDX 0x70000028
#endif

#ifndef DT_MIPS_OPTIONS
#define DT_MIPS_OPTIONS		 0x70000029 /* Address of .options.  */
#endif

#ifndef DT_MIPS_INTERFACE
#define DT_MIPS_INTERFACE	 0x7000002a /* Address of .interface.  */
#endif

#ifndef DT_MIPS_DYNSTR_ALIGN
#define DT_MIPS_DYNSTR_ALIGN	 0x7000002b
#endif

#ifndef DT_MIPS_INTERFACE_SIZE
#define DT_MIPS_INTERFACE_SIZE	 0x7000002c /* Size of the .interface section. */
#endif

#ifndef DT_MIPS_RLD_TEXT_RESOLVE_ADDR
#define DT_MIPS_RLD_TEXT_RESOLVE_ADDR 0x7000002d /* Address of rld_text_rsolve
                                                    function stored in GOT.  */
#endif

#ifndef DT_MIPS_PERF_SUFFIX
#define DT_MIPS_PERF_SUFFIX	 0x7000002e /* Default suffix of dso to be added
                                           by rld on dlopen() calls.  */
#endif

#ifndef DT_MIPS_COMPACT_SIZE
#define DT_MIPS_COMPACT_SIZE	 0x7000002f /* (O32)Size of compact rel section. */
#endif

#ifndef DT_MIPS_GP_VALUE
#define DT_MIPS_GP_VALUE	 0x70000030 /* GP value for aux GOTs.  */
#endif

#ifndef DT_MIPS_AUX_DYNAMIC
#define DT_MIPS_AUX_DYNAMIC	 0x70000031 /* Address of aux .dynamic.  */
#endif



/* MIPS DT_MIPS_FLAGS flags */
#ifndef RHF_NONE
#define RHF_NONE				0	/* No flags */
#endif

#ifndef RHF_QUICKSTART
#define	RHF_QUICKSTART			(1 << 0)	/* Object may be quickstarted by loader */
#endif

#ifndef RHF_NOTPOT
#define	RHF_NOTPOT			(1 << 1)	/* Hash size not a power of two */
#endif

#ifndef RHF_NO_LIBRARY_REPLACEMENT
#define	RHF_NO_LIBRARY_REPLACEMENT	(1 << 2)	/* Use default system libraries only */
#endif

#ifndef RHF_NO_MOVE
#define	RHF_NO_MOVE			(1 << 3)	/* Do not relocate */
#endif

#ifndef RHF_SGI_ONLY
#define	RHF_SGI_ONLY			(1 << 4)	/* XXX */
#endif

#ifndef RHF_GUARANTEE_INIT
#define	RHF_GUARANTEE_INIT		(1 << 5)	/* XXX */
#endif

#ifndef RHF_DELTA_C_PLUS_PLUS
#define	RHF_DELTA_C_PLUS_PLUS		(1 << 6)	/* XXX */
#endif

#ifndef RHF_GUARANTEE_START_INIT
#define RHF_GUARANTEE_START_INIT	(1 << 7)	/* XXX */
#endif

#ifndef RHF_PIXIE
#define RHF_PIXIE			(1 << 8)	/* XXX */
#endif

#ifndef RHF_DEFAULT_DELAY_LOAD
#define RHF_DEFAULT_DELAY_LOAD		(1 << 9)	/* XXX */
#endif

#ifndef RHF_REQUICKSTART
#define RHF_REQUICKSTART		(1 << 10)	/* XXX */
#endif

#ifndef RHF_REQUICKSTARTED
#define RHF_REQUICKSTARTED		(1 << 11)	/* XXX */
#endif

#ifndef RHF_CORD
#define RHF_CORD			(1 << 12)	/* XXX */
#endif

#ifndef RHF_NO_UNRES_UNDEF
#define RHF_NO_UNRES_UNDEF		(1 << 13)	/* XXX */
#endif

#ifndef RHF_RLD_ORDER_SAFE
#define RHF_RLD_ORDER_SAFE		(1 << 14)	/* XXX */
#endif


/* Global pointer table defines */
#ifndef PT_MIPS_REGINFO
#define PT_MIPS_REGINFO			0x70000006
#endif

#ifndef SHT_MIPS_REGINFO
#define SHT_MIPS_REGINFO		0x70000000
#endif



