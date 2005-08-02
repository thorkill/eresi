/*
 *
 * Compatibility with the Alpha architecture
 *
 * Define all the relocation types that can be undefined on various OS
 *
 * Last update Sat Feb 26 20:57:27 2004 mayhem
 *
 */

/* Alpha relocations.  */

#ifndef EM_ALPHA
 #define EM_ALPHA 41
#endif

#ifndef EM_ALPHA_EXP
 #define EM_ALPHA_EXP 36902
#endif

#ifndef R_ALPHA_NONE
 #define R_ALPHA_NONE            0       /* No reloc */
#endif

#ifndef R_ALPHA_REFLONG
 #define R_ALPHA_REFLONG         1       /* Direct 32 bit */
#endif

#ifndef R_ALPHA_REFQUAD
 #define R_ALPHA_REFQUAD         2       /* Direct 64 bit */
#endif

#ifndef R_ALPHA_GPREL32
 #define R_ALPHA_GPREL32         3       /* GP relative 32 bit */
#endif

#ifndef R_ALPHA_LITERAL
 #define R_ALPHA_LITERAL         4       /* GP relative 16 bit w/optimization */
#endif

#ifndef R_ALPHA_LITUSE
 #define R_ALPHA_LITUSE          5       /* Optimization hint for LITERAL */
#endif

#ifndef R_ALPHA_GPDISP
 #define R_ALPHA_GPDISP          6       /* Add displacement to GP */
#endif

#ifndef R_ALPHA_BRADDR
 #define R_ALPHA_BRADDR          7       /* PC+4 relative 23 bit shifted */
#endif

#ifndef R_ALPHA_HINT
 #define R_ALPHA_HINT            8       /* PC+4 relative 16 bit shifted */
#endif

#ifndef R_ALPHA_SREL16
 #define R_ALPHA_SREL16          9       /* PC relative 16 bit */
#endif

#ifndef R_ALPHA_SREL32
 #define R_ALPHA_SREL32          10      /* PC relative 32 bit */
#endif

#ifndef R_ALPHA_SREL64
 #define R_ALPHA_SREL64          11      /* PC relative 64 bit */
#endif

#ifndef R_ALPHA_OP_PUSH
 #define R_ALPHA_OP_PUSH         12      /* OP stack push */
#endif

#ifndef R_ALPHA_OP_STORE
 #define R_ALPHA_OP_STORE        13      /* OP stack pop and store */
#endif

#ifndef R_ALPHA_OP_PSUB
 #define R_ALPHA_OP_PSUB         14      /* OP stack subtract */
#endif

#ifndef R_ALPHA_OP_PRSHIFT
 #define R_ALPHA_OP_PRSHIFT      15      /* OP stack right shift */
#endif

#ifndef R_ALPHA_GPVALUE
 #define R_ALPHA_GPVALUE         16      /* Global Pointer value */
#endif

#ifndef R_ALPHA_IMMED_GP_16
 #define R_ALPHA_IMMED_GP_16	 19
#endif

#ifndef R_ALPHA_GPRELHIGH
 #define R_ALPHA_GPRELHIGH       17      /* GP relative 32 bit, high 16 bits */
#endif

#ifndef R_ALPHA_GPRELLOW
 #define R_ALPHA_GPRELLOW        18      /* GP relative 32 bit, low 16 bits */
#endif

#ifndef R_ALPHA_GPREL16
 #define R_ALPHA_GPREL16         19      /* GP relative 16 bit */
#endif

#ifndef R_ALPHA_IMMED_GP_HI32
 #define R_ALPHA_IMMED_GP_HI32   20      /* Unknown */
#endif

#ifndef R_ALPHA_IMMED_SCN_HI32
 #define R_ALPHA_IMMED_SCN_HI32  21      /* Unknown */
#endif

#ifndef R_ALPHA_IMMED_BR_HI32
 #define R_ALPHA_IMMED_BR_HI32   22      /* Unknown */
#endif

#ifndef R_ALPHA_IMMED_LO32
 #define R_ALPHA_IMMED_LO32      23      /* Unknown */
#endif

#ifndef R_ALPHA_COPY
 #define R_ALPHA_COPY            24      /* Copy symbol at runtime */
#endif

#ifndef R_ALPHA_GLOB_DAT
 #define R_ALPHA_GLOB_DAT        25      /* Create GOT entry */
#endif

#ifndef R_ALPHA_JMP_SLOT
 #define R_ALPHA_JMP_SLOT        26      /* Create PLT entry */
#endif

#ifndef R_ALPHA_RELATIVE
 #define R_ALPHA_RELATIVE        27      /* Adjust by program base */
#endif

#ifndef R_ALPHA_TLS_GD_HI
 #define R_ALPHA_TLS_GD_HI       28
#endif

#ifndef R_ALPHA_TLSGD
 #define R_ALPHA_TLSGD           29
#endif

#ifndef R_ALPHA_TLS_LDM
 #define R_ALPHA_TLS_LDM         30
#endif

#ifndef R_ALPHA_DTPMOD64
 #define R_ALPHA_DTPMOD64        31
#endif

#ifndef R_ALPHA_GOTDTPREL
 #define R_ALPHA_GOTDTPREL       32
#endif

#ifndef R_ALPHA_DTPREL64
 #define R_ALPHA_DTPREL64        33
#endif

#ifndef R_ALPHA_DTPRELHI
 #define R_ALPHA_DTPRELHI        34
#endif

#ifndef R_ALPHA_DTPRELLO
 #define R_ALPHA_DTPRELLO        35
#endif

#ifndef R_ALPHA_DTPREL16
 #define R_ALPHA_DTPREL16        36
#endif

#ifndef R_ALPHA_GOTTPREL
 #define R_ALPHA_GOTTPREL        37
#endif

#ifndef R_ALPHA_TPREL64
 #define R_ALPHA_TPREL64         38
#endif

#ifndef R_ALPHA_TPRELHI
 #define R_ALPHA_TPRELHI         39
#endif

#ifndef R_ALPHA_TPRELLO
 #define R_ALPHA_TPRELLO         40
#endif

#ifndef  R_ALPHA_TPREL16
 #define R_ALPHA_TPREL16         41
#endif
