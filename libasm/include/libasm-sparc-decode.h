/*
** libasm-sparc-decode.h for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 05:02:37 2005 #!HATE#@!
** Last update Tue Jun 14 06:06:32 2005 #!HATE#@!
**
** $Id: libasm-sparc-decode.h,v 1.4 2007/03/07 16:45:34 thor Exp $
**
*/

#ifndef LIBASM_SPARC_DECODE_H_
#define LIBASM_SPARC_DECODE_H_

/*
 * sparc instruction formats
 */

struct s_decode_pbranch {
  uint32_t	op:2;
  uint32_t	a:1;
  uint32_t	cond:4; /* TODO: fix the branches decode to use cc's */
  uint32_t	op2:3;
  uint32_t cc1:1;
  uint32_t cc0:1;
  uint32_t p:1;
  uint32_t	immediate:19;
  
  uint32_t imm;
  uint32_t cc;
};

struct s_decode_rbranch {
  uint32_t	op:2;
  uint32_t	a:1;
  uint32_t zero:1;
  uint32_t	rcond:3;
  uint32_t	op2:3;
  uint32_t d16hi:2;
  uint32_t p:1;
  uint32_t rs1:5;
  uint32_t	d16lo:14;
  
  uint32_t d16;
};

struct s_decode_branch {
  uint32_t	op:2;
  uint32_t	a:1;
  uint32_t	cond:4;
  uint32_t	op2:3;
  uint32_t	immediate:22;
  
  uint32_t imm;
  uint32_t rd;
};

struct s_decode_call {
  uint32_t	op:2;
  uint32_t	disp30:30;
  
  uint32_t displacement;
};

struct s_decode_format3 {
  uint32_t	op:2;
  uint32_t	rd:5;
  uint32_t	op3:6;
  uint32_t	rs1:5;
  uint32_t	i:1;
  uint32_t	none:8;
  uint32_t	rs2:5;
  
  uint32_t imm;
  uint32_t imm10;
  uint32_t rcond;
  uint32_t shcnt;
  uint32_t opf;
  uint32_t cc; // as used by FCMP*
  uint32_t opf_cc; // as used by FMOV(s,d,q)cc
  uint32_t cond; // as used by FMOV(s,d,q)cc
};

struct s_decode_format4 {
  uint32_t	op:2;
  uint32_t	rd:5;
  uint32_t	op3:6;
  uint32_t	rs1:5;
  uint32_t	i:1;
  uint32_t cc1:1;
  uint32_t cc0:1;
  uint32_t	none:6;
  uint32_t	rs2:5;
  
  uint32_t cond;
  uint32_t cc2;
  uint32_t cc;  
  uint32_t imm;
  uint32_t sw_trap;
};

#endif
