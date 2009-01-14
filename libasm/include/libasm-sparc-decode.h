/*
** libasm-sparc-decode.h for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 05:02:37 2005 #!HATE#@!
** Last update Tue Jun 14 06:06:32 2005 #!HATE#@!
**
** $Id$
**
*/

#ifndef LIBASM_SPARC_DECODE_H_
#define LIBASM_SPARC_DECODE_H_

/*
 * sparc instruction formats
 */

struct s_decode_pbranch {
  u_int32_t	op:2;
  u_int32_t	a:1;
  u_int32_t	cond:4; /* TODO: fix the branches decode to use cc's */
  u_int32_t	op2:3;
  u_int32_t cc1:1;
  u_int32_t cc0:1;
  u_int32_t p:1;
  u_int32_t	immediate:19;
  
  u_int32_t imm;
  u_int32_t cc;
};

struct s_decode_rbranch {
  u_int32_t	op:2;
  u_int32_t	a:1;
  u_int32_t zero:1;
  u_int32_t	rcond:3;
  u_int32_t	op2:3;
  u_int32_t d16hi:2;
  u_int32_t p:1;
  u_int32_t rs1:5;
  u_int32_t	d16lo:14;
  
  u_int32_t d16;
};

struct s_decode_branch {
  u_int32_t	op:2;
  u_int32_t	a:1;
  u_int32_t	cond:4;
  u_int32_t	op2:3;
  u_int32_t	immediate:22;
  
  u_int32_t imm;
  u_int32_t rd;
};

struct s_decode_call {
  u_int32_t	op:2;
  u_int32_t	disp30:30;
  
  u_int32_t displacement;
};

struct s_decode_format3 {
  u_int32_t	op:2;
  u_int32_t	rd:5;
  u_int32_t	op3:6;
  u_int32_t	rs1:5;
  u_int32_t	i:1;
  u_int32_t	none:8;
  u_int32_t	rs2:5;
  
  u_int32_t imm;
  u_int32_t imm10;
  u_int32_t rcond;
  u_int32_t shcnt;
  u_int32_t opf;
  u_int32_t cc; // as used by FCMP*
  u_int32_t opf_cc; // as used by FMOV(s,d,q)cc
  u_int32_t cond; // as used by FMOV(s,d,q)cc
};

struct s_decode_format4 {
  u_int32_t	op:2;
  u_int32_t	rd:5;
  u_int32_t	op3:6;
  u_int32_t	rs1:5;
  u_int32_t	i:1;
  u_int32_t cc1:1;
  u_int32_t cc0:1;
  u_int32_t	none:6;
  u_int32_t	rs2:5;
  
  u_int32_t cond;
  u_int32_t cc2;
  u_int32_t cc;  
  u_int32_t imm;
  u_int32_t sw_trap;
};

#endif
