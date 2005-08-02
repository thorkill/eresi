/*
 *
 * Compatibility with the HPPA architecture
 *
 * Define all the relocation types that can be undefined on various OS
 *
 * Last update Sat Feb 26 20:57:27 2004 mayhem
 *
 */

/* Legal values for e_flags field of Elf32_Ehdr.  */

#ifndef EF_PARISC_TRAPNIL
#define EF_PARISC_TRAPNIL	0x00010000 /* Trap nil pointer dereference.  */
#endif

#ifndef EF_PARISC_EXT
#define EF_PARISC_EXT		0x00020000 /* Program uses arch. extensions. */
#endif

#ifndef EF_PARISC_LSB
#define EF_PARISC_LSB		0x00040000 /* Program expects little endian. */
#endif

#ifndef EF_PARISC_WIDE
#define EF_PARISC_WIDE		0x00080000 /* Program expects wide mode.  */
#endif

#ifndef EF_PARISC_NO_KABP
#define EF_PARISC_NO_KABP	0x00100000 /* No kernel assisted branch
					      prediction.  */
#endif

#ifndef EF_PARISC_LAZYSWAP
#define EF_PARISC_LAZYSWAP	0x00400000 /* Allow lazy swapping.  */
#endif

#ifndef EF_PARISC_ARCH
#define EF_PARISC_ARCH		0x0000ffff /* Architecture version.  */
#endif

/* Defined values for `e_flags & EF_PARISC_ARCH' are:  */

#ifndef EFA_PARISC_1_0
#define EFA_PARISC_1_0		    0x020b /* PA-RISC 1.0 big-endian.  */
#endif

#ifndef EFA_PARISC_1_1
#define EFA_PARISC_1_1		    0x0210 /* PA-RISC 1.1 big-endian.  */
#endif

#ifndef EFA_PARISC_2_0
#define EFA_PARISC_2_0		    0x0214 /* PA-RISC 2.0 big-endian.  */
#endif

/* Additional section indeces.  */

#ifndef SHN_PARISC_ANSI_COMMON
#define SHN_PARISC_ANSI_COMMON	0xff00	   /* Section for tenatively declared
					      symbols in ANSI C.  */
#endif

#ifndef SHN_PARISC_HUGE_COMMON
#define SHN_PARISC_HUGE_COMMON	0xff01	   /* Common blocks in huge model.  */
#endif

/* Legal values for sh_type field of Elf32_Shdr.  */

#ifndef SHT_PARISC_EXT
#define SHT_PARISC_EXT		0x70000000 /* Contains product specific ext. */
#endif

#ifndef SHT_PARISC_UNWIND
#define SHT_PARISC_UNWIND	0x70000001 /* Unwind information.  */
#endif

#ifndef SHT_PARISC_DOC
#define SHT_PARISC_DOC		0x70000002 /* Debug info for optimized code. */
#endif

/* Legal values for sh_flags field of Elf32_Shdr.  */

#ifndef SHF_PARISC_SHORT
#define SHF_PARISC_SHORT	0x20000000 /* Section with short addressing. */
#endif

#ifndef SHF_PARISC_HUGE
#define SHF_PARISC_HUGE		0x40000000 /* Section far from gp.  */
#endif

#ifndef SHF_PARISC_SBP
#define SHF_PARISC_SBP		0x80000000 /* Static branch prediction code. */
#endif

/* Legal values for ST_TYPE subfield of st_info (symbol type).  */
#ifndef STT_PARISC_MILLICODE
#define STT_PARISC_MILLICODE	13	/* Millicode function entry point.  */
#endif

#ifndef STT_HP_OPAQUE
#define STT_HP_OPAQUE		(STT_LOOS + 0x1)
#endif

#ifndef STT_HP_STUB
#define STT_HP_STUB		(STT_LOOS + 0x2)
#endif

/* HPPA relocs.  */
#ifndef R_PARISC_NONE
#define R_PARISC_NONE		0	/* No reloc.  */
#endif

#ifndef R_PARISC_DIR32
#define R_PARISC_DIR32		1	/* Direct 32-bit reference.  */
#endif

#ifndef R_PARISC_DIR21L
#define R_PARISC_DIR21L		2	/* Left 21 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR17R
#define R_PARISC_DIR17R		3	/* Right 17 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR17F
#define R_PARISC_DIR17F		4	/* 17 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR14R
#define R_PARISC_DIR14R		6	/* Right 14 bits of eff. address.  */
#endif

#ifndef R_PARISC_PCREL32
#define R_PARISC_PCREL32	9	/* 32-bit rel. address.  */
#endif

#ifndef R_PARISC_PCREL21L
#define R_PARISC_PCREL21L	10	/* Left 21 bits of rel. address.  */
#endif

#ifndef R_PARISC_PCREL17R
#define R_PARISC_PCREL17R	11	/* Right 17 bits of rel. address.  */
#endif

#ifndef R_PARISC_PCREL17F
#define R_PARISC_PCREL17F	12	/* 17 bits of rel. address.  */
#endif

#ifndef R_PARISC_PCREL14R
#define R_PARISC_PCREL14R	14	/* Right 14 bits of rel. address.  */
#endif

#ifndef R_PARISC_DPREL21L
#define R_PARISC_DPREL21L	18	/* Left 21 bits of rel. address.  */
#endif

#ifndef R_PARISC_DPREL14R
#define R_PARISC_DPREL14R	22	/* Right 14 bits of rel. address.  */
#endif

#ifndef R_PARISC_GPREL21L
#define R_PARISC_GPREL21L	26	/* GP-relative, left 21 bits.  */
#endif

#ifndef R_PARISC_GPREL14R
#define R_PARISC_GPREL14R	30	/* GP-relative, right 14 bits.  */
#endif

#ifndef R_PARISC_LTOFF21L
#define R_PARISC_LTOFF21L	34	/* LT-relative, left 21 bits.  */
#endif

#ifndef R_PARISC_LTOFF14R
#define R_PARISC_LTOFF14R	38	/* LT-relative, right 14 bits.  */
#endif

#ifndef R_PARISC_SECREL32
#define R_PARISC_SECREL32	41	/* 32 bits section rel. address.  */
#endif

#ifndef R_PARISC_SEGBASE
#define R_PARISC_SEGBASE	48	/* No relocation, set segment base.  */
#endif

#ifndef R_PARISC_SEGREL32
#define R_PARISC_SEGREL32	49	/* 32 bits segment rel. address.  */
#endif

#ifndef R_PARISC_PLTOFF21L
#define R_PARISC_PLTOFF21L	50	/* PLT rel. address, left 21 bits.  */
#endif

#ifndef R_PARISC_PLTOFF14R
#define R_PARISC_PLTOFF14R	54	/* PLT rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_LTOFF_FPTR32
#define R_PARISC_LTOFF_FPTR32	57	/* 32 bits LT-rel. function pointer. */
#endif

#ifndef R_PARISC_LTOFF_FPTR21L
#define R_PARISC_LTOFF_FPTR21L	58	/* LT-rel. fct ptr, left 21 bits. */
#endif

#ifndef R_PARISC_LTOFF_FPTR14R
#define R_PARISC_LTOFF_FPTR14R	62	/* LT-rel. fct ptr, right 14 bits. */
#endif

#ifndef R_PARISC_FPTR64
#define R_PARISC_FPTR64		64	/* 64 bits function address.  */
#endif

#ifndef R_PARISC_PLABEL32
#define R_PARISC_PLABEL32	65	/* 32 bits function address.  */
#endif

#ifndef R_PARISC_PCREL64
#define R_PARISC_PCREL64	72	/* 64 bits PC-rel. address.  */
#endif

#ifndef R_PARISC_PCREL22F
#define R_PARISC_PCREL22F	74	/* 22 bits PC-rel. address.  */
#endif

#ifndef R_PARISC_PCREL14WR
#define R_PARISC_PCREL14WR	75	/* PC-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_PCREL14DR
#define R_PARISC_PCREL14DR	76	/* PC rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_PCREL16F
#define R_PARISC_PCREL16F	77	/* 16 bits PC-rel. address.  */
#endif

#ifndef R_PARISC_PCREL16WF
#define R_PARISC_PCREL16WF	78	/* 16 bits PC-rel. address.  */
#endif

#ifndef R_PARISC_PCREL16DF
#define R_PARISC_PCREL16DF	79	/* 16 bits PC-rel. address.  */
#endif

#ifndef R_PARISC_DIR64
#define R_PARISC_DIR64		80	/* 64 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR14WR
#define R_PARISC_DIR14WR	83	/* 14 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR14DR
#define R_PARISC_DIR14DR	84	/* 14 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR16F
#define R_PARISC_DIR16F		85	/* 16 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR16WF
#define R_PARISC_DIR16WF	86	/* 16 bits of eff. address.  */
#endif

#ifndef R_PARISC_DIR16DF
#define R_PARISC_DIR16DF	87	/* 16 bits of eff. address.  */
#endif

#ifndef R_PARISC_GPREL64
#define R_PARISC_GPREL64	88	/* 64 bits of GP-rel. address.  */
#endif

#ifndef R_PARISC_GPREL14WR
#define R_PARISC_GPREL14WR	91	/* GP-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_GPREL14DR
#define R_PARISC_GPREL14DR	92	/* GP-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_GPREL16F
#define R_PARISC_GPREL16F	93	/* 16 bits GP-rel. address.  */
#endif

#ifndef R_PARISC_GPREL16WF
#define R_PARISC_GPREL16WF	94	/* 16 bits GP-rel. address.  */
#endif

#ifndef R_PARISC_GPREL16DF
#define R_PARISC_GPREL16DF	95	/* 16 bits GP-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF64
#define R_PARISC_LTOFF64	96	/* 64 bits LT-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF14WR
#define R_PARISC_LTOFF14WR	99	/* LT-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_LTOFF14DR
#define R_PARISC_LTOFF14DR	100	/* LT-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_LTOFF16F
#define R_PARISC_LTOFF16F	101	/* 16 bits LT-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF16WF
#define R_PARISC_LTOFF16WF	102	/* 16 bits LT-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF16DF
#define R_PARISC_LTOFF16DF	103	/* 16 bits LT-rel. address.  */
#endif

#ifndef R_PARISC_SECREL64
#define R_PARISC_SECREL64	104	/* 64 bits section rel. address.  */
#endif

#ifndef R_PARISC_SEGREL64
#define R_PARISC_SEGREL64	112	/* 64 bits segment rel. address.  */
#endif

#ifndef R_PARISC_PLTOFF14WR
#define R_PARISC_PLTOFF14WR	115	/* PLT-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_PLTOFF14DR
#define R_PARISC_PLTOFF14DR	116	/* PLT-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_PLTOFF16F
#define R_PARISC_PLTOFF16F	117	/* 16 bits LT-rel. address.  */
#endif

#ifndef R_PARISC_PLTOFF16WF
#define R_PARISC_PLTOFF16WF	118	/* 16 bits PLT-rel. address.  */
#endif

#ifndef R_PARISC_PLTOFF16DF
#define R_PARISC_PLTOFF16DF	119	/* 16 bits PLT-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF_FPTR64
#define R_PARISC_LTOFF_FPTR64	120	/* 64 bits LT-rel. function ptr.  */
#endif

#ifndef R_PARISC_LTOFF_FPTR14WR
#define R_PARISC_LTOFF_FPTR14WR	123	/* LT-rel. fct. ptr., right 14 bits. */
#endif

#ifndef R_PARISC_LTOFF_FPTR14DR
#define R_PARISC_LTOFF_FPTR14DR	124	/* LT-rel. fct. ptr., right 14 bits. */
#endif

#ifndef R_PARISC_LTOFF_FPTR16F
#define R_PARISC_LTOFF_FPTR16F	125	/* 16 bits LT-rel. function ptr.  */
#endif

#ifndef R_PARISC_LTOFF_FPTR16WF
#define R_PARISC_LTOFF_FPTR16WF	126	/* 16 bits LT-rel. function ptr.  */
#endif

#ifndef R_PARISC_LTOFF_FPTR16DF
#define R_PARISC_LTOFF_FPTR16DF	127	/* 16 bits LT-rel. function ptr.  */
#endif

#ifndef R_PARISC_LORESERVE
#define R_PARISC_LORESERVE	128
#endif

#ifndef R_PARISC_COPY
#define R_PARISC_COPY		128	/* Copy relocation.  */
#endif

#ifndef R_PARISC_IPLT
#define R_PARISC_IPLT		129	/* Dynamic reloc, imported PLT */
#endif

#ifndef R_PARISC_EPLT
#define R_PARISC_EPLT		130	/* Dynamic reloc, exported PLT */
#endif

#ifndef R_PARISC_TPREL32
#define R_PARISC_TPREL32	153	/* 32 bits TP-rel. address.  */
#endif

#ifndef R_PARISC_TPREL21L
#define R_PARISC_TPREL21L	154	/* TP-rel. address, left 21 bits.  */
#endif

#ifndef R_PARISC_TPREL14R
#define R_PARISC_TPREL14R	158	/* TP-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_LTOFF_TP21L
#define R_PARISC_LTOFF_TP21L	162	/* LT-TP-rel. address, left 21 bits. */
#endif

#ifndef R_PARISC_LTOFF_TP14R
#define R_PARISC_LTOFF_TP14R	166	/* LT-TP-rel. address, right 14 bits.*/
#endif

#ifndef R_PARISC_LTOFF_TP14F
#define R_PARISC_LTOFF_TP14F	167	/* 14 bits LT-TP-rel. address.  */
#endif

#ifndef R_PARISC_TPREL64
#define R_PARISC_TPREL64	216	/* 64 bits TP-rel. address.  */
#endif

#ifndef R_PARISC_TPREL14WR
#define R_PARISC_TPREL14WR	219	/* TP-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_TPREL14DR
#define R_PARISC_TPREL14DR	220	/* TP-rel. address, right 14 bits.  */
#endif

#ifndef R_PARISC_TPREL16F
#define R_PARISC_TPREL16F	221	/* 16 bits TP-rel. address.  */
#endif

#ifndef R_PARISC_TPREL16WF
#define R_PARISC_TPREL16WF	222	/* 16 bits TP-rel. address.  */
#endif

#ifndef R_PARISC_TPREL16DF
#define R_PARISC_TPREL16DF	223	/* 16 bits TP-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF_TP64
#define R_PARISC_LTOFF_TP64	224	/* 64 bits LT-TP-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF_TP14WR
#define R_PARISC_LTOFF_TP14WR	227	/* LT-TP-rel. address, right 14 bits.*/
#endif

#ifndef R_PARISC_LTOFF_TP14DR
#define R_PARISC_LTOFF_TP14DR	228	/* LT-TP-rel. address, right 14 bits.*/
#endif

#ifndef R_PARISC_LTOFF_TP16F
#define R_PARISC_LTOFF_TP16F	229	/* 16 bits LT-TP-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF_TP16WF
#define R_PARISC_LTOFF_TP16WF	230	/* 16 bits LT-TP-rel. address.  */
#endif

#ifndef R_PARISC_LTOFF_TP16DF
#define R_PARISC_LTOFF_TP16DF	231	/* 16 bits LT-TP-rel. address.  */
#endif

#ifndef R_PARISC_HIRESERVE
#define R_PARISC_HIRESERVE	255
#endif

/* Legal values for p_type field of Elf32_Phdr/Elf64_Phdr.  */

#ifndef PT_HP_TLS
#define PT_HP_TLS		(PT_LOOS + 0x0)
#endif

#ifndef PT_HP_CORE_NONE
#define PT_HP_CORE_NONE		(PT_LOOS + 0x1)
#endif

#ifndef PT_HP_CORE_VERSION
#define PT_HP_CORE_VERSION	(PT_LOOS + 0x2)
#endif

#ifndef PT_HP_CORE_KERNEL
#define PT_HP_CORE_KERNEL	(PT_LOOS + 0x3)
#endif

#ifndef PT_HP_CORE_COMM
#define PT_HP_CORE_COMM		(PT_LOOS + 0x4)
#endif

#ifndef PT_HP_CORE_PROC
#define PT_HP_CORE_PROC		(PT_LOOS + 0x5)
#endif

#ifndef PT_HP_CORE_LOADABLE
#define PT_HP_CORE_LOADABLE	(PT_LOOS + 0x6)
#endif

#ifndef PT_HP_CORE_STACK
#define PT_HP_CORE_STACK	(PT_LOOS + 0x7)
#endif

#ifndef PT_HP_CORE_SHM
#define PT_HP_CORE_SHM		(PT_LOOS + 0x8)
#endif

#ifndef PT_HP_CORE_MMF
#define PT_HP_CORE_MMF		(PT_LOOS + 0x9)
#endif

#ifndef PT_HP_PARALLEL
#define PT_HP_PARALLEL		(PT_LOOS + 0x10)
#endif

#ifndef PT_HP_FASTBIND
#define PT_HP_FASTBIND		(PT_LOOS + 0x11)
#endif

#ifndef PT_HP_OPT_ANNOT
#define PT_HP_OPT_ANNOT		(PT_LOOS + 0x12)
#endif

#ifndef PT_HP_HSL_ANNOT
#define PT_HP_HSL_ANNOT		(PT_LOOS + 0x13)
#endif

#ifndef PT_HP_STACK
#define PT_HP_STACK		(PT_LOOS + 0x14)
#endif

#ifndef PT_PARISC_ARCHEXT
#define PT_PARISC_ARCHEXT	0x70000000
#endif

#ifndef PT_PARISC_UNWIND
#define PT_PARISC_UNWIND	0x70000001
#endif

/* Legal values for p_flags field of Elf32_Phdr/Elf64_Phdr.  */

#ifndef PF_PARISC_SBP
#define PF_PARISC_SBP		0x08000000
#endif

#ifndef PF_HP_PAGE_SIZE
#define PF_HP_PAGE_SIZE		0x00100000
#endif

#ifndef PF_HP_FAR_SHARED
#define PF_HP_FAR_SHARED	0x00200000
#endif

#ifndef PF_HP_NEAR_SHARED
#define PF_HP_NEAR_SHARED	0x00400000
#endif

#ifndef PF_HP_CODE
#define PF_HP_CODE		0x01000000
#endif

#ifndef PF_HP_MODIFY
#define PF_HP_MODIFY		0x02000000
#endif

#ifndef PF_HP_LAZYSWAP
#define PF_HP_LAZYSWAP		0x04000000
#endif

#ifndef PF_HP_SBP
#define PF_HP_SBP		0x08000000
#endif


