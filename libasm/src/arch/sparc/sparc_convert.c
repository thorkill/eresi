/*
** sparc_convert.c for  in /hate/home/hate/code/libasm
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Mon Apr 25 01:01:26 2005 #!HATE#@!
** Last update Tue Jun 14 09:18:36 2005 #!HATE#@!
**
** $Id: sparc_convert.c,v 1.6 2007/06/16 20:24:25 strauss Exp $
**
*/

/*
 * Note: during the development on this code, I got kinda
 * confused about the efficiency of this endian-specific-code
 * approach. I'm not sure if this is doing what it's supposed
 * to, but I'm too stupid to draw a conclusion. So, please,
 * someone take a look at the code, fix whatever needs to
 * be fixed and remove this note. kthx. 
 * 
 * Strauss
 */  

#include <libasm.h>

void sparc_convert_pbranch(struct s_decode_pbranch	*opcode, unsigned char *buf)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
    int converted;
  
    memcpy(&converted, buf, 4);
    opcode->op = (converted >> 30) & 0x3;
    opcode->a = (converted >> 29) & 0x1;
    opcode->cond = (converted >> 25) & 0xf;
    opcode->op2 = (converted >> 22) & 0x7;
    opcode->cc1 = (converted >> 21) & 0x1;
    opcode->cc0 = (converted >> 20) & 0x1;
    opcode->p = (converted >> 19) & 0x1;
    opcode->immediate = converted & 0x7FFFF;
  #else
    memcpy(opcode, buf, 4);
  #endif
  
  opcode->cc = (opcode->cc1 << 1) | opcode->cc0;
  
  if (opcode->immediate & 0x40000)
   	opcode->imm = opcode->immediate | 0xFFFC0000;
  else
   	opcode->imm = opcode->immediate;
}

void sparc_convert_rbranch(struct s_decode_rbranch *opcode,	unsigned char *buf)
{
  int d16;
  
  #if __BYTE_ORDER == __LITTLE_ENDIAN
    int converted;
  
    memcpy(&converted, buf, 4);
    opcode->op = (converted >> 30) & 0x3;
    opcode->a = (converted >> 29) & 0x1;
    opcode->zero = (converted >> 28) & 0x1;
    opcode->rcond = (converted >> 25) & 0x7;
    opcode->op2 = (converted >> 22) & 0x7;    
    opcode->d16hi = (converted >> 20) & 0x3;
    opcode->p = (converted >> 19) & 0x1;
    opcode->rs1 = (converted >> 14) & 0x1f;
    opcode->d16lo = converted & 0x3FFF;    
  #else
    memcpy(opcode, buf, 4);
  #endif
  
  d16 = (opcode->d16hi << 14) | (opcode->d16lo);
  if (d16 & 0x8000)
  	opcode->d16 = d16 | 0xFFFF0000;
  else
  	opcode->d16 = d16;
}

void sparc_convert_branch(struct s_decode_branch *opcode, unsigned char *buf)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
    int converted;
  
    memcpy(&converted, buf, 4);
    opcode->op = (converted >> 30) & 0x3;
    opcode->a = (converted >> 29) & 0x1;
    opcode->cond = (converted >> 25) & 0xf;
    opcode->op2 = (converted >> 22) & 0x7;
    opcode->immediate = converted & 0x3FFFFF;
  #else
    memcpy(opcode, buf, 4);
  #endif

  opcode->rd = (opcode->a << 4) | opcode->cond;
  
  if (opcode->immediate & 0x200000)
   	opcode->imm = opcode->immediate | 0xFFE00000;
  else
   	opcode->imm = opcode->immediate;
}

void sparc_convert_call(struct s_decode_call *opcode,	unsigned char *buf)
{
  #if __BYTE_ORDER == __LITTLE_ENDIAN
    int converted;
  
  	memcpy(&converted, buf, 4);
  	opcode->op = (converted >> 30) & 0x3;
  	opcode->disp30 = (converted & 0x3FFFFFFF);
  #else
  	memcpy(opcode, buf, 4);
  #endif
  
  if (opcode->disp30 & 0x20000000)
    opcode->displacement = opcode->disp30 | 0xC0000000;
  else
  	opcode->displacement = opcode->disp30;
}

void sparc_convert_format3(struct s_decode_format3 *opcode,	unsigned char *buf)
{
  int shcnt, immediate10; 
  int immediate;
  
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  	int converted;
  
  	memcpy(&converted, buf, 4);
  	opcode->op = (converted >> 30) & 0x3;
  	opcode->rd = (converted >> 25) & 0x1f;
  	opcode->op3 = (converted >> 19) & 0x3f;
  	opcode->rs1 = (converted >> 14) & 0x1f;
  	opcode->i = (converted >> 13) & 0x1;
  	opcode->none = (converted >> 5) & 0xff;
  	opcode->rs2 = converted & 0x1f;
  #else
  	memcpy(opcode, buf, 4);
  #endif
  
  /* The following code sets the immediate and the shcnt values 
   * (abstracting whether the shcnt value is for 32 or 64 bits)
   */
  immediate = (opcode->none << 5) | (opcode->rs2);  
  	  	
  if (immediate & 0x1000)
    opcode->imm = 0xFFFFF000 | immediate;
  else
    opcode->imm = immediate;
      
  immediate10 = immediate & 0x3ff;
  if (immediate10 & 0x200)
    opcode->imm10 = 0xFFFFFC00 | immediate10;
  else
    opcode->imm10 = immediate10;
      
  shcnt = immediate & 0x3f;
  /* (immediate & 0x1fff == the 'x' bit */
  if ((immediate & 0x1fff) == 0)
    shcnt &= 0x1f;
  	  
  opcode->shcnt = shcnt;
  
  opcode->rcond = (opcode->imm & 0x1c00) >> 10;
  opcode->opf = (opcode->i << 8) | opcode->none;
  opcode->opf_cc = (opcode->opf & 0x1c0) >> 6;
  opcode->cc = opcode->rd & 0x3;
  opcode->cond = opcode->rs1 & 0xf;
}

void sparc_convert_format4(struct s_decode_format4 *opcode, unsigned char *buf)
{
  int immediate;
  
  #if __BYTE_ORDER == __LITTLE_ENDIAN
  	int converted;
  
  	memcpy(&converted, buf, 4);
  	opcode->op = (converted >> 30) & 0x3;
  	opcode->rd = (converted >> 25) & 0x1f;
  	opcode->op3 = (converted >> 19) & 0x3f;
  	opcode->rs1 = (converted >> 14) & 0x1f;
  	opcode->i = (converted >> 13) & 0x1;
  	opcode->cc1 = (converted >> 12) & 0x1;
  	opcode->cc0 = (converted >>11) & 0x1;
  	opcode->none = (converted >> 5) & 0x3f;
  	opcode->rs2 = converted & 0x1f;
  #else
  	memcpy(opcode, buf, 4);
  #endif 
  
  opcode->cond = opcode->rs1 & 0xf;
  opcode->cc2 = opcode->rs1 & 0x10;
  opcode->cc = (opcode->cc2 << 2) | (opcode->cc1 << 1) | opcode->cc0;
  
  opcode->sw_trap = ((opcode->none & 0x3) << 5) | opcode->rs2;
     
  immediate = (opcode->none << 5) | (opcode->rs2);
  	
  if (immediate & 0x400)
    opcode->imm = 0xFFFFF800 | immediate;
  else
    opcode->imm = immediate;
}
