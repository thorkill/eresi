/*
** sparc_convert.c for  in /hate/home/hate/code/libasm
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Mon Apr 25 01:01:26 2005 #!HATE#@!
** Last update Tue Jun 14 09:18:36 2005 #!HATE#@!
*/

#include <libasm.h>

  
  
void	sparc_convert_branch(struct s_decode_branch	*opcode,
			     u_char *buf, asm_processor *proc)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  int				converted;
  #endif
  
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 30) & 0x3;
  opcode->a = (converted >> 29) & 0x1;
  opcode->cond = (converted >> 25) & 0xf;
  opcode->op2 = (converted >> 22);
  opcode->immediate = converted & 0x3FFFFF;
  #else
  memcpy(opcode, buf, 4);
  #endif
}

void	sparc_convert_sethi(struct s_decode_sethi	*opcode,
			    u_char *buf, asm_processor *proc)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  int				converted;
  #endif

  #if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 30) & 0x3;
  opcode->rd = (converted >> 25) & 0x1f;
  opcode->sethi = (converted >> 22) & 0x7;
  opcode->immediate = (converted & 0x3FFFFF);
  #else
  memcpy(opcode, buf, 4);
  #endif    
}


void	sparc_convert_call(struct s_decode_call	*opcode,
			   u_char *buf, asm_processor *proc)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  int			converted;
  #endif

  #if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 30) & 0x3;
  opcode->displacement = (converted & 0x3FFFFFFF);
  #else
  memcpy(opcode, buf, 4);
  #endif
}


void	sparc_convert_format3_rs(struct s_decode_f3rs	*opcode,
				 u_char *buf, 
				 asm_processor *proc)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  int			converted;
  #endif

  #if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 30) & 0x3;
  opcode->rd = (converted >> 25) & 0x1f;
  opcode->op3 = (converted >> 19) & 0x3f;
  opcode->rs1 = (converted >> 14) & 0x1f;
  opcode->zero = (converted >> 13) & 1;
  opcode->none = (converted >> 5) & 0xff;
  opcode->rs2 = converted & 0x1f;
  #else
  memcpy(opcode, buf, 4);
  #endif
    
}


void	sparc_convert_format3_imm(struct s_decode_f3imm	*opcode,
				  u_char *buf,
				  asm_processor *proc)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  int			converted;
  #endif

  #if __BYTE_ORDER == __LITTLE_ENDIAN
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 30) & 0x3;
  opcode->rd = (converted >> 25) & 0x1f;
  opcode->op3 = (converted >> 19) & 0x3f;
  opcode->rs1 = (converted >> 14) & 0x1f;
  opcode->sign = (converted >> 13) & 1;
  opcode->immediate = converted & 0x1ffff;
  #else
  memcpy(opcode, buf, 4);
  #endif
  
}



