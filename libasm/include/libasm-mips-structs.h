/**
 * $Id: libasm-mips-structs.h,v 1.1 2007/01/23 09:53:03 heroine Exp $
 *
 * fix and fill                                                                                                                                  
 *          - Adam 'pi3' Zabrocki
 *
 */
#ifndef LIBASM_MIPS_STRUCTS_H
#define LIBASM_MIPS_STRUCTS_H

/**
 * Structure used to decode format reg instructions
 */
struct s_mips_decode_reg
{
  u_int32_t	op:6;	/*! opcode field		*/
  u_int32_t	rs:5;	/*! 1st source register		*/
  u_int32_t	rt:5;	/*! 2nd source register		*/
  u_int32_t	rd:5;	/*! destination register	*/
  u_int32_t	sa:5;	/*! shift amount (shift instr)	*/
  u_int32_t	fn:6;	/*! function to perform		*/
};

/**
 * Structure used to decode format immediate instructions
 */
struct s_mips_decode_imm
{
  u_int32_t	op:6;
  u_int32_t	rs:5;
  u_int32_t	rt:5;
  u_int32_t	im:16;
};

/**
 * Structure used to decode format jump instructions
 */
struct s_mips_decode_jump
{
  u_int32_t	op:6;	/*! opcode field		*/
  u_int32_t	ta:26;	/*! target to jump to		*/
};

/**
 * Structure used to decode format trap instructions
 */
struct s_mips_decode_trap
{
  u_int32_t	op:6;
  u_int32_t	rs:5;
  u_int32_t	rt:5;
  u_int32_t	code:10;
  u_int32_t	fn:6;
};

/**
 * Structure used to decode format mov* instructions
 */
struct s_mips_decode_mov
{
  u_int32_t	op:6;
  u_int32_t	rs:5;
  u_int32_t	cc:3;
  u_int32_t	b1:1;
  u_int32_t	tf:1;
  u_int32_t	rd:5;
  u_int32_t	b2:5;
  u_int32_t	fn:6;
};

/**
 * Structure used to decode some coprocesor2 instructions
 */
struct s_mips_decode_cop2
{
  u_int32_t	op:6;
  u_int32_t	rs:5;
  u_int32_t	rt:5;
  u_int32_t	rd:5;
  u_int32_t	fn:8;
  u_int32_t	sl:3;
};

#define s_mips_decode_priv s_mips_decode_cop2

/**
 * Structure used to decode COP1X format instructions
 */
struct s_mips_decode_cop1x
{
  u_int32_t	op:6;
  u_int32_t	bs:5;
  u_int32_t	in:5;
  u_int32_t	f1:5;
  u_int32_t	f2:5;
  u_int32_t	fn:3;
  u_int32_t	fmt:3;
};

#endif
