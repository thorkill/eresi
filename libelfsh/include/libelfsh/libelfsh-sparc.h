/*
 *
 * Compatibility with the Sparc architecture
 *
 * Define all the relocation types that can be undefined on various OS
 *
 * Last update Sat Feb 27 16:57:02 2005 mayhem
 *
 */


#ifndef EM_SPARC
 #define EM_SPARC 2
#endif

#ifndef EM_SPARC32PLUS
 #define EM_SPARC32PLUS 18
#endif

#ifndef EM_SPARCV9
 #define EM_SPARCV9 43
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


/* Extended SPARC64 relocation types */
#ifndef R_SPARC_PLT32
 #define R_SPARC_PLT32   24
#endif
#ifndef R_SPARC_HIPLT22
 #define R_SPARC_HIPLT22 25
#endif
#ifndef R_SPARC_LOPLT10
 #define R_SPARC_LOPLT10 26
#endif
#ifndef R_SPARC_PCPLT32
 #define R_SPARC_PCPLT32 27
#endif
#ifndef R_SPARC_PCPLT22
 #define R_SPARC_PCPLT22 28
#endif
#ifndef R_SPARC_PCPLT10
 #define R_SPARC_PCPLT10 29
#endif
#ifndef R_SPARC_10
 #define R_SPARC_10      30
#endif
#ifndef R_SPARC_11
 #define R_SPARC_11      31
#endif
#ifndef R_SPARC_64
 #define R_SPARC_64      32
#endif
#ifndef R_SPARC_OLO10
 #define R_SPARC_OLO10   33
#endif
#ifndef R_SPARC_HH22
 #define R_SPARC_HH22    34
#endif
#ifndef R_SPARC_HM10
 #define R_SPARC_HM10    35
#endif
#ifndef R_SPARC_LM22
 #define R_SPARC_LM22    36
#endif
#ifndef R_SPARC_PC_HH22
 #define R_SPARC_PC_HH22 37
#endif
#ifndef R_SPARC_PC_HM10
 #define R_SPARC_PC_HM10 38
#endif
#ifndef R_SPARC_PC_LM22
 #define R_SPARC_PC_LM22 39
#endif
#ifndef R_SPARC_WDISP16
 #define R_SPARC_WDISP16 40
#endif 
#ifndef R_SPARC_WDISP19
 #define R_SPARC_WDISP19 41
#endif
#ifndef R_SPARC_7
 #define R_SPARC_7       43
#endif
#ifndef R_SPARC_5
 #define R_SPARC_5       44
#endif
#ifndef R_SPARC_6
 #define R_SPARC_6       45
#endif
#ifndef R_SPARC_DISP64
 #define R_SPARC_DISP64  46
#endif
#ifndef R_SPARC_PLT64
 #define R_SPARC_PLT64   47
#endif 
#ifndef R_SPARC_HIX22
 #define R_SPARC_HIX22   48
#endif
#ifndef R_SPARC_LOX10
 #define R_SPARC_LOX10   49
#endif
#ifndef R_SPARC_H44
 #define R_SPARC_H44     50
#endif
#ifndef R_SPARC_M44
 #define R_SPARC_M44     51
#endif
#ifndef R_SPARC_L44
 #define R_SPARC_L44     52
#endif
#ifndef R_SPARC_REGISTE
 #define R_SPARC_REGISTER 53
#endif
#ifndef R_SPARC_UA64
 #define R_SPARC_UA64    54
#endif
#ifndef R_SPARC_UA16
 #define R_SPARC_UA16    55
#endif
