#ifndef _LIBASM_ARM_H_
#define _LIBASM_ARM_H_

/* Registration functions */
int asm_register_arm();

char *asm_arm_display_instr(asm_instr *instr, eresi_Addr addr);

/* Instruction hHandlers */
int asm_arm_adc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_add(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_and(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_b(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bic(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bl(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cdp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cmn(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cmp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_eor(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldm(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrbt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrsb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrsh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mcr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mla(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mov(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mrc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mrs(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_msr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mul(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mvn(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_orr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_rsb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_rsc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_sbc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smlal(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smull(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_stc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_stm(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_str(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strbt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_sub(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swi(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swpb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_teq(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_tst(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_umlal(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_umull(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);

/***
 * Instruction list
 * 
 * Last instruction must be ASM_ARM_BAD
 */

enum e_arm_instr
  {
    ASM_ARM_,

    ASM_ARM_ADC,
    ASM_ARM_ADD,
    ASM_ARM_AND,
    ASM_ARM_B,
    ASM_ARM_BIC,
    ASM_ARM_BL,
    ASM_ARM_CDP,
    ASM_ARM_CMN,
    ASM_ARM_CMP,
    ASM_ARM_EOR,
    ASM_ARM_LDC,
    ASM_ARM_LDM,
    ASM_ARM_LDR,
    ASM_ARM_LDRB,
    ASM_ARM_LDRBT,
    ASM_ARM_LDRH,
    ASM_ARM_LDRSB,
    ASM_ARM_LDRSH,
    ASM_ARM_LDRT,
    ASM_ARM_MCR,
    ASM_ARM_MLA,
    ASM_ARM_MOV,
    ASM_ARM_MRC,
    ASM_ARM_MRS,
    ASM_ARM_MSR,
    ASM_ARM_MUL,
    ASM_ARM_MVN,
    ASM_ARM_ORR,
    ASM_ARM_RSB,
    ASM_ARM_RSC,
    ASM_ARM_SBC,
    ASM_ARM_SMLAL,
    ASM_ARM_SMULL,
    ASM_ARM_STC,
    ASM_ARM_STM,
    ASM_ARM_STR,
    ASM_ARM_STRB,
    ASM_ARM_STRBT,
    ASM_ARM_STRH,
    ASM_ARM_STRT,
    ASM_ARM_SUB,
    ASM_ARM_SWI,
    ASM_ARM_SWP,
    ASM_ARM_SWPB,
    ASM_ARM_TEQ,
    ASM_ARM_TST,
    ASM_ARM_UMLAL,
    ASM_ARM_UMULL,
    ASM_ARM_BAD,
  };

extern char *arm_instr_list[ASM_ARM_BAD + 1];

#endif /* _LIBASM_ARM_H_ */
