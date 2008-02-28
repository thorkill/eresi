/**
 * $Id: mips_convert.c,v 1.1 2007/01/23 09:53:03 heroine Exp $
 *
 * Integrated to vectors
 *            - Adam 'pi3' Zabrocki
 *
 */

#include <libasm.h>

void	mips_convert_format_r(struct s_mips_decode_reg	*opcode,
			      u_char *buf)
{
  u_int32_t	converted;
  
  if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->rt = (converted >> 16) & 0x1F;
     opcode->rd = (converted >> 11) & 0x1F;
     opcode->sa = (converted >> 6) & 0x1F;
     opcode->fn = (converted >> 0) & 0x3F;
  } else {
     printf("[CONV_R] Where am I ?!?!?!\n");
     exit(-1);
  }

}


void	mips_convert_format_i(struct s_mips_decode_imm *opcode,
			      u_char *buf)
{
  u_int32_t	converted;

  if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->rt = (converted >> 16) & 0x1F;
//     opcode->im = (converted >> 0) & 0xFFFFFF;
     opcode->im = (converted >> 0) & 0xFFFF;
  } else {
     printf("[CONV_I] Where am I ?!?!?!\n");
     exit(-1);
  }

}


void	mips_convert_format_j(struct s_mips_decode_jump *opcode,
			      u_char *buf)
{

  u_int32_t	converted;

  if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->ta = (converted >> 0) & 0x3FFFFFF;
  } else {
     printf("[CONV_J] Where am I ?!?!?!\n");
     exit(-1);
  }

}

void	mips_convert_format_t(struct s_mips_decode_trap *opcode,
			      u_char *buf)
{
  u_int32_t	converted;

  if (asm_config_get_endian() == ASM_CONFIG_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == ASM_CONFIG_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->rt = (converted >> 16) & 0x1F;
     opcode->code = (converted >> 6) & 0x3FF;
     opcode->fn = (converted >> 0) & 0x3F;
  } else {
     printf("[CONV_T] Where am I ?!?!?!\n");
     exit(-1);
  }

}
