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
  uint32_t	op:6;	/*! opcode field		*/
  uint32_t	rs:5;	/*! 1st source register		*/
  uint32_t	rt:5;	/*! 2nd source register		*/
  uint32_t	rd:5;	/*! destination register	*/
  uint32_t	sa:5;	/*! shift amount (shift instr)	*/
  uint32_t	fn:6;	/*! function to perform		*/
};

/**
 * Structure used to decode format immediate instructions
 */
struct s_mips_decode_imm
{
  uint32_t	op:6;
  uint32_t	rs:5;
  uint32_t	rt:5;
  uint32_t	im:16;
};

/**
 * Structure used to decode format jump instructions
 */
struct s_mips_decode_jump
{
  uint32_t	op:6;	/*! opcode field		*/
  uint32_t	ta:26;	/*! target to jump to		*/
};

/**
 * Structure used to decode format trap instructions
 */
struct s_mips_decode_trap
{
  uint32_t	op:6;
  uint32_t	rs:5;
  uint32_t	rt:5;
  uint32_t	code:10;
  uint32_t	fn:6;
};

/**
 * Structure used to decode format mov* instructions
 */
struct s_mips_decode_mov
{
  uint32_t	op:6;
  uint32_t	rs:5;
  uint32_t	cc:3;
  uint32_t	b1:1;
  uint32_t	tf:1;
  uint32_t	rd:5;
  uint32_t	b2:5;
  uint32_t	fn:6;
};

/**
 * Structure used to decode some coprocesor2 instructions
 */
struct s_mips_decode_cop2
{
  uint32_t	op:6;
  uint32_t	rs:5;
  uint32_t	rt:5;
  uint32_t	rd:5;
  uint32_t	fn:8;
  uint32_t	sl:3;
};

#define s_mips_decode_priv s_mips_decode_cop2

/**
 * Structure used to decode COP1X format instructions
 */
struct s_mips_decode_cop1x
{
  uint32_t	op:6;
  uint32_t	bs:5;
  uint32_t	in:5;
  uint32_t	f1:5;
  uint32_t	f2:5;
  uint32_t	fn:3;
  uint32_t	fmt:3;
};

#endif
