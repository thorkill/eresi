/*
** tables_sparc.c for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 05:23:16 2005 #!HATE#@!
** Last update Thu Jun 16 05:39:25 2005 #!HATE#@!
*/

#include <libasm.h>

int	sparc_op2_table[64] = {
  ASM_SP_ADD,
  ASM_SP_AND,
  ASM_SP_OR,
  ASM_SP_XOR,
  ASM_SP_SUB,
  ASM_SP_ANDN,
  ASM_SP_ORN,
  ASM_SP_XNOR,

  ASM_SP_ADDC,
  ASM_SP_MULX,
  ASM_SP_UMUL,
  ASM_SP_SMUL,
  ASM_SP_SUBC,
  ASM_SP_UDIVX,
  ASM_SP_UDIV,
  ASM_SP_SDIV,

  ASM_SP_ADDCC,
  ASM_SP_ANDCC,
  ASM_SP_ORCC,
  ASM_SP_XORCC,
  ASM_SP_SUBCC,
  ASM_SP_ANDNCC,
  ASM_SP_ORNCC,
  ASM_SP_XNORCC,

  ASM_SP_ADDCCC,
  ASM_SP_,
  ASM_SP_UMULCC,
  ASM_SP_SMULCC,
  ASM_SP_SUBCCC,
  ASM_SP_,
  ASM_SP_UDIVCC,
  ASM_SP_SDIVCC,


  ASM_SP_TADDCC,
  ASM_SP_TSUBCC,
  ASM_SP_TADDCCTV,
  ASM_SP_TSUBCCTV,
  ASM_SP_MULSCC,
  ASM_SP_SLL, /* SLL, SLLX */
  ASM_SP_SRL, /* SRL, SRLX */
  ASM_SP_SRA, /* SRA, SRAX */

  ASM_SP_RD, /* RD*(-PR), MEMBAR, STBAR */
  ASM_SP_,
  ASM_SP_RDPR,
  ASM_SP_FLUSHW,
  ASM_SP_MOVA, /* MOVcc */
  ASM_SP_SDIVX,
  ASM_SP_POPC,
  ASM_SP_MOVRZ, /* MOVr */

  ASM_SP_WR, /* WR*(-PR), SIR */
  ASM_SP_SAVED, /* SAVED, RESTORED */
  ASM_SP_WRPR,
  ASM_SP_,
  ASM_SP_FMOVS, /* FPop1 */
  ASM_SP_FMOVS, /* FPop2 */
  ASM_SP_IMPDEP1,
  ASM_SP_IMPDEP2,

  ASM_SP_JMPL,
  ASM_SP_RETURN,
  ASM_SP_TA,
  ASM_SP_FLUSH,
  ASM_SP_SAVE,
  ASM_SP_RESTORE,
  ASM_SP_DONE, /* DONE, RETRY */
  ASM_SP_
};

int sparc_op3_table[64] = {
  ASM_SP_LDUW,
  ASM_SP_LDUB,
  ASM_SP_LDUH,
  ASM_SP_LDD,
  ASM_SP_STW,
  ASM_SP_STB,
  ASM_SP_STH,
  ASM_SP_STD,

  ASM_SP_LDSW,
  ASM_SP_LDSB,
  ASM_SP_LDSH,
  ASM_SP_LDX,
  ASM_SP_,
  ASM_SP_LDSTUB,
  ASM_SP_STX,
  ASM_SP_SWAP,

  ASM_SP_LDUWA,
  ASM_SP_LDUBA,
  ASM_SP_LDUHA,
  ASM_SP_LDDA,
  ASM_SP_STWA,
  ASM_SP_STBA,
  ASM_SP_STHA,
  ASM_SP_STDA,

  ASM_SP_LDSWA,
  ASM_SP_LDSBA,
  ASM_SP_LDSHA,
  ASM_SP_LDXA,
  ASM_SP_,
  ASM_SP_LDSTUBA,
  ASM_SP_STXA,
  ASM_SP_SWAPA,

  

  ASM_SP_LDF,
  ASM_SP_LDFSR, /* LDFSR, LDXFSR */
  ASM_SP_LDQF,
  ASM_SP_LDDF,
  ASM_SP_STF,
  ASM_SP_STFSR, /* STFSR, STXFSR */
  ASM_SP_STQF,
  ASM_SP_STDF,

  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_PREFETCH,
  ASM_SP_,
  ASM_SP_,

  ASM_SP_LDFA,
  ASM_SP_,
  ASM_SP_LDQFA,
  ASM_SP_LDDFA,
  ASM_SP_STFA,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,

  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_CASA,
  ASM_SP_PREFETCHA,
  ASM_SP_CASXA,
  ASM_SP_
};

int	sparc_bcc_list[16] = {
  ASM_SP_BN,
  ASM_SP_BE,
  ASM_SP_BLE,
  ASM_SP_BL,
  ASM_SP_BLEU,
  ASM_SP_BCS,
  ASM_SP_BNEG,
  ASM_SP_BVS,
  ASM_SP_BA,
  ASM_SP_BNE,
  ASM_SP_BG,
  ASM_SP_BGE,
  ASM_SP_BGU,
  ASM_SP_BCC,
  ASM_SP_BPOS,
  ASM_SP_BVC  
};

int	sparc_brcc_list[8] = {
  ASM_SP_,
  ASM_SP_BRZ,
  ASM_SP_BRLEZ,
  ASM_SP_BRLZ,
  ASM_SP_,
  ASM_SP_BRNZ,
  ASM_SP_BRGZ,
  ASM_SP_BRGEZ  
};

int sparc_fbcc_list[16] = {
  ASM_SP_FBN,
  ASM_SP_FBNE,
  ASM_SP_FBLG,
  ASM_SP_FBUL,
  ASM_SP_FBL,
  ASM_SP_FBUG,
  ASM_SP_FBG,
  ASM_SP_FBU,
  ASM_SP_FBA,
  ASM_SP_FBE,
  ASM_SP_FBUE,
  ASM_SP_FBGE,
  ASM_SP_FBUGE,
  ASM_SP_FBLE,
  ASM_SP_FBULE,
  ASM_SP_FBO
};

int sparc_shift_list[6] = {
  ASM_SP_SLL,
  ASM_SP_SLLX,
  ASM_SP_SRL,
  ASM_SP_SRLX,
  ASM_SP_SRA,
  ASM_SP_SRAX,
};

int	sparc_movcc_list[16] = {
  ASM_SP_MOVN,
  ASM_SP_MOVE,
  ASM_SP_MOVLE,
  ASM_SP_MOVL,
  ASM_SP_MOVLEU,
  ASM_SP_MOVCS,
  ASM_SP_MOVNEG,
  ASM_SP_MOVVS,
  ASM_SP_MOVA,
  ASM_SP_MOVNE,
  ASM_SP_MOVG,
  ASM_SP_MOVGE,
  ASM_SP_MOVGU,
  ASM_SP_MOVCC,
  ASM_SP_MOVPOS,
  ASM_SP_MOVVC  
};

int sparc_movfcc_list[16] = {
  ASM_SP_MOVN,
  ASM_SP_MOVNE,
  ASM_SP_MOVLG,
  ASM_SP_MOVUL,
  ASM_SP_MOVL,
  ASM_SP_MOVUG,
  ASM_SP_MOVG,
  ASM_SP_MOVU,
  ASM_SP_MOVA,
  ASM_SP_MOVE,
  ASM_SP_MOVUE,
  ASM_SP_MOVGE,
  ASM_SP_MOVUGE,
  ASM_SP_MOVLE,
  ASM_SP_MOVULE,
  ASM_SP_MOVO
};

int	sparc_movr_list[8] = {
  ASM_SP_,
  ASM_SP_MOVRZ,
  ASM_SP_MOVRLEZ,
  ASM_SP_MOVRLZ,
  ASM_SP_,
  ASM_SP_MOVRNZ,
  ASM_SP_MOVRGZ,
  ASM_SP_MOVRGEZ  
};

int sparc_fpop1_list[256] = {
  ASM_SP_, /* 0x00 */
  ASM_SP_FMOVS,
  ASM_SP_FMOVD,
  ASM_SP_FMOVQ,
  ASM_SP_,
  ASM_SP_FNEGS,
  ASM_SP_FNEGD,
  ASM_SP_FNEGQ,
  ASM_SP_,
  ASM_SP_FABSS,
  ASM_SP_FABSD,
  ASM_SP_FABSQ,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x10 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x20 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_SQRTS,
  ASM_SP_SQRTD,
  ASM_SP_SQRTQ,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x30 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x40 */
  ASM_SP_FADDS,
  ASM_SP_FADDD,
  ASM_SP_FADDQ,
  ASM_SP_,
  ASM_SP_FSUBS,
  ASM_SP_FSUBD,
  ASM_SP_FSUBQ,
  ASM_SP_,
  ASM_SP_FMULS,
  ASM_SP_FMULD,
  ASM_SP_FMULQ,
  ASM_SP_,
  ASM_SP_FDIVS,
  ASM_SP_FDIVD,
  ASM_SP_FDIVQ,
  ASM_SP_, /* 0x50 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x60 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_FSMULD,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_FDMULQ,
  ASM_SP_,
  ASM_SP_, /* 0x70 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x80 */
  ASM_SP_FSTOX,
  ASM_SP_FDTOX,
  ASM_SP_FQTOX,
  ASM_SP_FXTOS,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_FXTOD,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_FXTOQ,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0x90 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0xA0 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0xB0 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0xC0 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_FITOS,
  ASM_SP_,
  ASM_SP_FDTOS,
  ASM_SP_FQTOS,
  ASM_SP_FITOD,
  ASM_SP_FSTOD,
  ASM_SP_,
  ASM_SP_FQTOD,
  ASM_SP_FITOQ,
  ASM_SP_FSTOQ,
  ASM_SP_FDTOQ,
  ASM_SP_,
  ASM_SP_, /* 0xD0 */
  ASM_SP_FSTOI,
  ASM_SP_FDTOI,
  ASM_SP_FQTOI,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0xE0 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_, /* 0xF0 */
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_,
  ASM_SP_
};

int sparc_fmovcc_list[48] = {
  ASM_SP_FMOVSN,
  ASM_SP_FMOVSE,
  ASM_SP_FMOVSLE,
  ASM_SP_FMOVSL,
  ASM_SP_FMOVSLEU,
  ASM_SP_FMOVSCS,
  ASM_SP_FMOVSNEG,
  ASM_SP_FMOVSVS,
  ASM_SP_FMOVSA,
  ASM_SP_FMOVSNE,
  ASM_SP_FMOVSG,
  ASM_SP_FMOVSGE,
  ASM_SP_FMOVSGU,
  ASM_SP_FMOVSCC,
  ASM_SP_FMOVSPOS,
  ASM_SP_FMOVSVC,
  ASM_SP_FMOVDN,
  ASM_SP_FMOVDE,
  ASM_SP_FMOVDLE,
  ASM_SP_FMOVDL,
  ASM_SP_FMOVDLEU,
  ASM_SP_FMOVDCS,
  ASM_SP_FMOVDNEG,
  ASM_SP_FMOVDVS,
  ASM_SP_FMOVDA,
  ASM_SP_FMOVDNE,
  ASM_SP_FMOVDG,
  ASM_SP_FMOVDGE,
  ASM_SP_FMOVDGU,
  ASM_SP_FMOVDCC,
  ASM_SP_FMOVDPOS,
  ASM_SP_FMOVDVC,
  ASM_SP_FMOVQN,
  ASM_SP_FMOVQE,
  ASM_SP_FMOVQLE,
  ASM_SP_FMOVQL,
  ASM_SP_FMOVQLEU,
  ASM_SP_FMOVQCS,
  ASM_SP_FMOVQNEG,
  ASM_SP_FMOVQVS,
  ASM_SP_FMOVQA,
  ASM_SP_FMOVQNE,
  ASM_SP_FMOVQG,
  ASM_SP_FMOVQGE,
  ASM_SP_FMOVQGU,
  ASM_SP_FMOVQCC,
  ASM_SP_FMOVQPOS,
  ASM_SP_FMOVQVC
};

int sparc_fmovfcc_list[48] = {
  ASM_SP_FMOVSN,
  ASM_SP_FMOVSNE,
  ASM_SP_FMOVSLG,
  ASM_SP_FMOVSUL,
  ASM_SP_FMOVSL,
  ASM_SP_FMOVSUG,
  ASM_SP_FMOVSG,
  ASM_SP_FMOVSU,
  ASM_SP_FMOVSA,
  ASM_SP_FMOVSE,
  ASM_SP_FMOVSUE,
  ASM_SP_FMOVSGE,
  ASM_SP_FMOVSUGE,
  ASM_SP_FMOVSLE,
  ASM_SP_FMOVSULE,
  ASM_SP_FMOVSO,
  ASM_SP_FMOVDN,
  ASM_SP_FMOVDNE,
  ASM_SP_FMOVDLG,
  ASM_SP_FMOVDUL,
  ASM_SP_FMOVDL,
  ASM_SP_FMOVDUG,
  ASM_SP_FMOVDG,
  ASM_SP_FMOVDU,
  ASM_SP_FMOVDA,
  ASM_SP_FMOVDE,
  ASM_SP_FMOVDUE,
  ASM_SP_FMOVDGE,
  ASM_SP_FMOVDUGE,
  ASM_SP_FMOVDLE,
  ASM_SP_FMOVDULE,
  ASM_SP_FMOVDO,
  ASM_SP_FMOVQN,
  ASM_SP_FMOVQNE,
  ASM_SP_FMOVQLG,
  ASM_SP_FMOVQUL,
  ASM_SP_FMOVQL,
  ASM_SP_FMOVQUG,
  ASM_SP_FMOVQG,
  ASM_SP_FMOVQU,
  ASM_SP_FMOVQA,
  ASM_SP_FMOVQE,
  ASM_SP_FMOVQUE,
  ASM_SP_FMOVQGE,
  ASM_SP_FMOVQUGE,
  ASM_SP_FMOVQLE,
  ASM_SP_FMOVQULE,
  ASM_SP_FMOVQO
};

int sparc_fmovr_list[24] = {
  ASM_SP_,
  ASM_SP_FMOVRSZ,
  ASM_SP_FMOVRSLEZ,
  ASM_SP_FMOVRSLZ,
  ASM_SP_,
  ASM_SP_FMOVRSNZ,
  ASM_SP_FMOVRSGZ,
  ASM_SP_FMOVRSGEZ,
  ASM_SP_,
  ASM_SP_FMOVRDZ,
  ASM_SP_FMOVRDLEZ,
  ASM_SP_FMOVRDLZ,
  ASM_SP_,
  ASM_SP_FMOVRDNZ,
  ASM_SP_FMOVRDGZ,
  ASM_SP_FMOVRDGEZ,
  ASM_SP_,
  ASM_SP_FMOVRQZ,
  ASM_SP_FMOVRQLEZ,
  ASM_SP_FMOVRQLZ,
  ASM_SP_,
  ASM_SP_FMOVRQNZ,
  ASM_SP_FMOVRQGZ,
  ASM_SP_FMOVRQGEZ
};

int sparc_fcmp_list[8] = {
  ASM_SP_,
  ASM_SP_FCMPS,
  ASM_SP_FCMPD,
  ASM_SP_FCMPQ,
  ASM_SP_,
  ASM_SP_FCMPES,
  ASM_SP_FCMPED,
  ASM_SP_FCMPEQ
};

char *sparc_instr_list[ASM_SP_BAD + 1] = {
  "(unimpl)",
  "add", /* ADD */
  "addcc", /* ADDcc */
  "addc", /* ADDC */
  "addccc", /* ADDCcc */
  "and", /* AND */
  "andcc", /* ANDcc */
  "andn", /* ANDN */
  "andncc", /* ANDNcc */
  "ba", /* BPcc, Bicc */
  "bn",
  "bne",
  "be",
  "bg",
  "ble",
  "bge",
  "bl",
  "bgu",
  "bleu",
  "bcc",
  "bcs",
  "bpos",
  "bneg",
  "bvc",
  "bvs",
  "brz", /* BPr */
  "brlez",
  "brlz",
  "brnz",
  "brgz",
  "brgez",
  "call", /* CALL */
  "casa", /* CASA */
  "casxa", /* CASXA */
  "done", /* DONE */
  "fabss", /* FABS(s,d,q) */
  "fabsd",
  "fabsq",
  "fadds", /* FADD(s,d,q) */
  "faddd",
  "faddq",
  "fba", /* FBfcc, FBPfcc */
  "fbn",
  "fbu",
  "fbg",
  "fbug",
  "fbl",
  "fbul",
  "fblg",
  "fbne",
  "fbe",
  "fbue",
  "fbge",
  "fbuge",
  "fble",
  "fbule",
  "fbo",
  "fcmps", /* FCMP(s,d,q) */
  "fcmpd",
  "fcmpq",
  "fcmpes", /* FCMPE(s,d,q) */
  "fcmped",
  "fcmpeq",
  "fdivs", /* FDIV(s,d,q) */
  "fdivd",
  "fdivq",
  "fdmulq", /* FdMULq */
  "fitos", /* FiTO(s,d,q) */
  "fitod",
  "fitoq",
  "flush", /* FLUSH */
  "flushw", /* FLUSHW */
  "fmovs", /* FMOV(s,d,q) */
  "fmovd",
  "fmovq",
  "fmovsa", /* FMOV(s,d,q)cc */
  "fmovsn",
  "fmovsne",
  "fmovse",
  "fmovsg",
  "fmovsle",
  "fmovsge",
  "fmovsl",
  "fmovsgu",
  "fmovsleu",
  "fmovscc",
  "fmovscs",
  "fmovspos",
  "fmovsneg",
  "fmovsvc",
  "fmovsvs",
  "fmovsu",
  "fmovsug",
  "fmovsul",
  "fmovslg",
  "fmovsue",
  "fmovsuge",
  "fmovsule",
  "fmovso",
  "fmovda",
  "fmovdn",
  "fmovdne",
  "fmovde",
  "fmovdg",
  "fmovdle",
  "fmovdge",
  "fmovdl",
  "fmovdgu",
  "fmovdleu",
  "fmovdcc",
  "fmovdcs",
  "fmovdpos",
  "fmovdneg",
  "fmovdvc",
  "fmovdvs",
  "fmovdu",
  "fmovdug",
  "fmovdul",
  "fmovdlg",
  "fmovdue",
  "fmovduge",
  "fmovdule",
  "fmovdo",
  "fmovqa",
  "fmovqn",
  "fmovqne",
  "fmovqe",
  "fmovqg",
  "fmovqle",
  "fmovqge",
  "fmovql",
  "fmovqgu",
  "fmovqleu",
  "fmovqcc",
  "fmovqcs",
  "fmovqpos",
  "fmovqneg",
  "fmovqvc",
  "fmovqvs",
  "fmovqu",
  "fmovqug",
  "fmovqul",
  "fmovqlg",
  "fmovque",
  "fmovquge",
  "fmovqule",
  "fmovo",
  "fmovrsz", /* FMOV(s,d,q)r */
  "fmovrslez",
  "fmovrslz",
  "fmovrsnz",
  "fmovrsgz",
  "fmovrsgez",
  "fmovrdz",
  "fmovrdlez",
  "fmovrdlz",
  "fmovrdnz",
  "fmovrdgz",
  "fmovrdgez",
  "fmovrqz",
  "fmovrqlez",
  "fmovrqlz",
  "fmovrqnz",
  "fmovrqgz",
  "fmovrqgez",
  "fmuls", /* FMUL(s,d,q) */
  "fmuld",
  "fmulq",
  "fnegs", /* FNEG(s,d,q) */
  "fnegd",
  "fnegq",
  "fsmuld", /* FsMULd */
  "fsqrts", /* FSQRT(s,d,q) */
  "fsqrtd",
  "fsqrtq",
  "fstoi", /* F(s,d,q)TOi */
  "fdtoi",
  "fqtoi",
  "fstod", /* F(s,d,q)TO(s,d,q) */
  "fstoq",
  "fdtos",
  "fdtoq",
  "fqtos",
  "fqtod",
  "fstox", /* F(s,d,q)TOx */
  "fdtox",
  "fqtox",
  "fsubs", /* FSUB(s,d,q) */
  "fsubd",
  "fsubq",
  "fxtos", /* FxTO(s,d,q) */
  "fxtod",
  "fxtoq",
  "illtrap", /* ILLTRAP */
  "impdep1", /* IMPDEP1 and IMPDEP2 are not real names, these are 
  			  * implementation-specific instruction. Maybe we should let
  			  * the user specify a name for these instructions?
  			  */ 
  "impdep2", 
  "jmpl", /* JMPL, RET, RETL */
  "ldd", /* LDD */
  "ldda", /* LDDA */
  "ldd", /* LDFF */
  "ldda", /* LDDFA */
  "ld", /* LDF */
  "lda", /* LDFA */
  "ld", /* LDFSR */
  "ldq", /* LDQF */
  "ldqa", /* LDQFA */
  "ldsb", /* LDSB */
  "ldsba", /* LDSBA */
  "ldsh", /* LDSH */
  "ldsha", /* LDSHA */
  "ldstub", /* LDSTUB */
  "ldstuba", /* LDSTUBA */
  "ldsw", /* LDSW */
  "ldswa", /* LDSWA */
  "ldub", /* LDUB */
  "lduba", /*LDUBA */
  "lduh", /* LDUH */
  "lduha", /* LDUHA */
  "lduw", /* LDUW */
  "lduwa", /* LDUWA */
  "ldx", /* LDX */
  "ldxa", /* LDXA */
  "ldx", /* LDXFSR */
  "membar", /* MEMBAR */
  "mova", /* MOVcc */
  "movn",
  "movne",
  "move",
  "movg",
  "movle",
  "movge",
  "movl",
  "movgu",
  "movleu",
  "movcc",
  "movcs",
  "movpos",
  "movneg",
  "movvc",
  "movvs",
  "movu",
  "movug",
  "movul",
  "movlg",
  "movue",
  "movuge",
  "movule",
  "movo",
  "movrz", /* MOVr */
  "movrlez",
  "movrlz",
  "movrnz",
  "movrgz",
  "movrgez",
  "mulscc", /* MULScc */
  "mulx", /* MULX */
  "nop", /* NOP */
  "or", /* OR */
  "orcc", /* ORcc */
  "orn", /* ORC */
  "orncc", /* ORNcc */
  "popc", /* POPC */
  "prefetch", /* PREFETCH */
  "prefetcha", /* PREFETCHA */
  "rd", /* RDASI, RDASR, RDCCR, RDFPRS, RDPC, RDTICK, RDY */
  "rdpr", /* RDPR */
  "restore", /* RESTORE */
  "restored", /* RESTORED */
  "retry", /* RETRY */
  "return", /* RETURN */
  "save", /* SAVE */
  "saved", /* SAVED */
  "sdiv", /* SDIV */
  "sdivcc", /* SDIVcc */
  "sdivx", /* SDIVX */
  "sethi", /* SETHI */
  "sir", /* SIR */
  "sll", /* SLL */
  "sllx", /* SLLX */
  "smul", /* SMUL */
  "smulcc", /* SMULcc */
  "sqrts", /* SQRT(s,d,q) */
  "sqrtd",
  "sqrtq",
  "sra", /* SRA */
  "srax", /* SRAX */
  "srl", /* SRL */
  "srlx", /* SRLX */
  "stb", /* STB */
  "stba", /* STBA */
  "stbar", /* STBAR */
  "std", /* STD */
  "stda", /* STDA */
  "std", /* STDF */
  "stda", /* STDFA */
  "st", /* STF */
  "sta", /* STFA */
  "st", /* STFSR */
  "sth", /* STH */
  "stha", /* STHA */
  "stq", /* STQF */
  "stqa", /* STQFA */
  "stw", /* STW */
  "stwa", /* STWA */
  "stx", /* STX */
  "stxa", /* STXA */
  "stx", /* STXFSR */
  "sub", /* SUB */
  "subcc", /* SUBcc */
  "subc", /* SUBC */
  "subccc", /* SUBCcc */
  "swap", /* SWAP */
  "swapa", /* SWAPA */
  "taddcc", /* TADDcc */
  "taddcctv", /* TADDccTV */
  "ta", /* Tcc */
  "tn",
  "tne",
  "te",
  "tg",
  "tle",
  "tge",
  "tl",
  "tgu",
  "tleu",
  "tcc",
  "tcs",
  "tpos",
  "tneg",
  "tvc",
  "tvs",
  "tsubcc", /* TSUBcc */
  "tsubcctv", /* TSUBccTV */
  "udiv", /* UDIV */
  "udivcc", /* UDIVcc */
  "udivx", /* UDIVX */
  "umul", /* UMUL */
  "umulcc", /* UMULcc */
  "wr", /* WRASI, WRASR, WRCCR, WRFPRS, WRY */
  "wrpr", /* WRPR */  
  "xnor", /* XNOR */
  "xnorcc", /* XNORcc */
  "xor", /* XOR */
  "xorcc", /* XORcc */
  "(bad)"
};
