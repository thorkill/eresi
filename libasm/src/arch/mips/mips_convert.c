/**
 * $Id: mips_convert.c,v 1.1 2007-01-23 09:53:03 heroine Exp $
 *
 *
 *
 */

#include <libasm.h>

void	mips_convert_format_r(struct s_mips_decode_reg	*opcode,
			      u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_int32_t	converted;
  
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 26) & 0x3F;
  opcode->rs = (converted >> 21) & 0x1F;
  opcode->rt = (converted >> 16) & 0x1F;
  opcode->rd = (converted >> 11) & 0x1F;
  opcode->sa = (converted >> 6) & 0x1F;
  opcode->fn = (converted >> 0) & 0x3F;
#else
  memcpy(opcode, buf, 4);
#endif
}


void	mips_convert_format_i(struct s_mips_decode_imm *opcode,
			      u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_int32_t	converted;
  
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 26) & 0x3F;
  opcode->rs = (converted >> 21) & 0x1F;
  opcode->rt = (converted >> 16) & 0x1F;
  opcode->im = (converted >> 0) & 0xFFFFFF;
#else
  memcpy(opcode, buf, 4);
#endif
}


void	mips_convert_format_j(struct s_mips_decode_jump *opcode,
			      u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  u_int32_t	converted;
  
  memcpy(&converted, buf, 4);
  opcode->op = (converted >> 26) & 0x3F;
  opcode->ta = (converted >> 0) & 0x3FFFFFF;
#else
  memcpy(opcode, buf, 4);
#endif
}
