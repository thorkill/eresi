#ifndef LIBASM_ARM_DECODE_H
#define LIBASM_ARM_DECODE_H

/* B, BL, BLX(2) */
struct s_arm_decode_branch1
{
  u_int32_t	cond:4;
  u_int32_t	op:3;
  u_int32_t	l_h:1;
  u_int32_t	signed_imm:24;
};
/* BLX(2), BX */
struct s_arm_decode_branch2
{
  u_int32_t	cond:4;
  u_int32_t	none:24;
  u_int32_t	rm:4;
};

struct s_arm_decode_dataproc
{
  u_int32_t	cond:4;
  u_int32_t	op:2;
  u_int32_t	i:1;
  u_int32_t	op2:4;
  u_int32_t	s:1;
  u_int32_t	rn:4;
  u_int32_t	rd:4;
  u_int32_t	shifter_operand:12;

  u_int32_t	rm;
  u_int32_t	rs;
  u_int32_t	immed;
  u_int32_t	shift;
  u_int32_t	shift_imm;
  u_int32_t	rotate_imm;
  u_int32_t	reg_shift;
};

struct s_arm_decode_ldst
{
  u_int32_t	cond:4;
  u_int32_t	op:2;
  u_int32_t	reg_offset:1;
  u_int32_t	p:1;
  u_int32_t	u:1;
  u_int32_t	b:1;
  u_int32_t	w:1;
  u_int32_t	l:1;
  u_int32_t	rn:4;
  u_int32_t	rd:4;
  u_int32_t	imm_offset:12;

  u_int8_t 	rm;
  u_int8_t	shift_imm;
  u_int8_t	shift;
};

struct s_arm_decode_ldstmisc
{
  u_int32_t	cond:4;
  u_int32_t	op:2;
  u_int32_t	zero:1; //XXX
  u_int32_t	p:1;
  u_int32_t	u:1;
  u_int32_t	imm_offset:1;
  u_int32_t	w:1;
  u_int32_t	l:1;
  u_int32_t	rn:4;
  u_int32_t	rd:4;
  u_int32_t	immedH:4;
  u_int32_t	one1:1; //XXX
  u_int32_t	s:1;
  u_int32_t	h:1;
  u_int32_t	one2:1; //XXX
  u_int32_t	immedL_rm:4;
};

struct s_arm_decode_ldstmul
{
  u_int32_t	cond:4;
  u_int32_t	op:2;
  u_int32_t	zero:1; //XXX
  u_int32_t	p:1;
  u_int32_t	u:1;
  u_int32_t	s:1;
  u_int32_t	w:1;
  u_int32_t	l:1;
  u_int32_t	rn:4;
  u_int32_t	reg_list:16;
};

struct s_arm_decode_ldstcop
{
  u_int32_t	cond:4;
  u_int32_t	op:2;
  u_int32_t	zero:1;
  u_int32_t	p:1;
  u_int32_t	u:1;
  u_int32_t	n:1;
  u_int32_t	w:1;
  u_int32_t	l:1;
  u_int32_t	rn:4;
  u_int32_t	crd:4;
  u_int32_t	cp_nbr:4;
  u_int32_t	offset:8;
};

#endif /* LIBASM_ARM_DECODE_H */
