#ifndef _LIBASM_ARM_H_
#define _LIBASM_ARM_H_

/* Registration functions */
int asm_register_arm();

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

#endif /* _LIBASM_ARM_H_ */
