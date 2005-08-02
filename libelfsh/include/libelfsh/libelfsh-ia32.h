/*
 *
 * Compatibility with the IA32 architecture
 *
 * Define all the relocation types that can be undefined on various OS
 *
 * Last update Sat Feb 26 20:57:27 2004 yann malcom 
 *
 */


#ifndef EM_386
  #define EM_386 3
#endif



/* IA32 relocations.  */
#ifndef R_386_NONE
 #define R_386_NONE         0        /* No reloc */
#endif

#ifndef R_386_32
 #define R_386_32           1        /* Direct 32 bit  */
#endif

#ifndef R_386_PC32
 #define R_386_PC32         2        /* PC relative 32 bit */
#endif

#ifndef R_386_GOT32
 #define R_386_GOT32        3        /* 32 bit GOT entry */
#endif

#ifndef R_386_PLT32
 #define R_386_PLT32        4        /* 32 bit PLT address */
#endif

#ifndef R_386_COPY
 #define R_386_COPY         5        /* Copy symbol at runtime */
#endif

#ifndef R_386_GLOB_DAT
 #define R_386_GLOB_DAT     6        /* Create GOT entry */
#endif

#ifndef R_386_JMP_SLOT
 #define R_386_JMP_SLOT     7        /* Create PLT entry */
#endif

#ifndef R_386_RELATIVE
 #define R_386_RELATIVE     8        /* Adjust by program base */
#endif

#ifndef R_386_GOTOFF
 #define R_386_GOTOFF       9        /* 32 bit offset to GOT */
#endif

#ifndef R_386_GOTPC
 #define R_386_GOTPC        10       /* 32 bit PC relative offset to GOT */
#endif

#ifndef R_386_32PLT
 #define R_386_32PLT        11
#endif

#ifndef R_386_TLS_TPOFF
 #define R_386_TLS_TPOFF    14       /* Offset in static TLS block */
#endif

#ifndef R_386_TLS_IE
 #define R_386_TLS_IE       15       /* Address of GOT entry for static TLS
					block offset */
#endif

#ifndef R_386_TLS_GOTIE
 #define R_386_TLS_GOTIE    16       /* GOT entry for static TLS block
					offset */
#endif

#ifndef R_386_TLS_LE
 #define R_386_TLS_LE       17       /* Offset relative to static TLS
					block */
#endif

#ifndef R_386_TLS_GD
 #define R_386_TLS_GD       18       /* Direct 32 bit for GNU version of
					general dynamic thread local data */
#endif

#ifndef R_386_TLS_LDM
 #define R_386_TLS_LDM      19       /* Direct 32 bit for GNU version of
					local dynamic thread local data
					in LE code */
#endif

#ifndef R_386_16
 #define R_386_16           20
#endif

#ifndef R_386_PC16
 #define R_386_PC16         21
#endif

#ifndef R_386_8
 #define R_386_8            22
#endif

#ifndef R_386_PC8
 #define R_386_PC8          23
#endif

#ifndef R_386_TLS_GD_32
 #define R_386_TLS_GD_32    24       /* Direct 32 bit for general dynamic
					thread local data */
#endif

#ifndef R_386_TLS_GD_PUSH
 #define R_386_TLS_GD_PUSH  25       /* Tag for pushl in GD TLS code */
#endif

#ifndef R_386_TLS_GD_CALL
 #define R_386_TLS_GD_CALL  26       /* Relocation for call to
					__tls_get_addr() */
#endif

#ifndef R_386_TLS_GD_POP
 #define R_386_TLS_GD_POP   27       /* Tag for popl in GD TLS code */
#endif

#ifndef R_386_TLS_LDM_32
 #define R_386_TLS_LDM_32   28       /* Direct 32 bit for local dynamic
					thread local data in LE code */
#endif

#ifndef R_386_TLS_LDM_PUSH
 #define R_386_TLS_LDM_PUSH 29       /* Tag for pushl in LDM TLS code */
#endif

#ifndef R_386_TLS_LDM_CALL
 #define R_386_TLS_LDM_CALL 30       /* Relocation for call to
					__tls_get_addr() in LDM code */
#endif

#ifndef R_386_TLS_LDM_POP
 #define R_386_TLS_LDM_POP  31       /* Tag for popl in LDM TLS code */
#endif

#ifndef R_386_TLS_LDO_32
 #define R_386_TLS_LDO_32   32       /* Offset relative to TLS block */
#endif

#ifndef R_386_TLS_IE_32
 #define R_386_TLS_IE_32    33       /* GOT entry for negated static TLS
					block offset */
#endif

#ifndef R_386_TLS_LE_32
 #define R_386_TLS_LE_32    34       /* Negated offset relative to static
					TLS block */
#endif

#ifndef R_386_TLS_DTPMOD32
 #define R_386_TLS_DTPMOD32 35       /* ID of module containing symbol */
#endif

#ifndef R_386_TLS_DTPOFF32
 #define R_386_TLS_DTPOFF32 36       /* Offset in TLS block */
#endif


