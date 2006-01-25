/*
** libasm-sparc-structs.h for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 05:02:37 2005 #!HATE#@!
** Last update Tue Jun 14 05:07:22 2005 #!HATE#@!
*/

#ifndef LIBASM_SPARC_STRUCTS_H_
#define LIBASM_SPARC_STRUCTS_H_

/*****
 * sparc format instruction
 *
 */


struct s_decode_branch {

  u_int32_t	op:2;
  u_int32_t	a:1;
  u_int32_t	cond:4;
  u_int32_t	op2:3;
  u_int32_t	immediate:22;
};

struct s_decode_sethi {
  u_int32_t	op:2;
  u_int32_t	rd:5;
  u_int32_t	sethi:3;
  u_int32_t	immediate:22;
};

struct s_decode_call {
  u_int32_t	op:2;
  u_int32_t	displacement:30;
};

struct s_decode_format3_rs {
  u_int32_t	op:2;
  u_int32_t	rd:5;
  u_int32_t	op3:6;
  u_int32_t	rs1:5;
  u_int32_t	zero:1;
  u_int32_t	none:8;
  u_int32_t	rs2:5;
};

struct s_format3_imm {
  u_int32_t	op:2;
  u_int32_t	rd:5;
  u_int32_t	op3:6;
  u_int32_t	rs1:5;
  u_int32_t	sign:1;
  u_int32_t	immediate:13;
};



#endif
