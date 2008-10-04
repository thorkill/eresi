/**
 * @file mips_convert.c
 * @brief Convert MIPS instruction to definied types and fill suitable structures.
 *
 * $Id: mips_convert.c,v 1.4 2008/03/13 15:46:27 heroine && pi3 Exp $
 *
 * Integrated to vectors
 *            - Adam 'pi3' Zabrocki
 *
 */

#include <libasm.h>

/**
 * @fn void    mips_convert_format_r(struct s_mips_decode_reg  *opcode, unsigned char *buf)
 * @brief Convert 'type r' instruction.
 *
 * Fill structure for 'type r' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_r(struct s_mips_decode_reg	*opcode,
			      unsigned char *buf)
{
  uint32_t	converted;
  
  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
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

/**
 * @fn void    mips_convert_format_i(struct s_mips_decode_imm *opcode, unsigned char *buf)
 * @brief Convert 'type i' instruction.
 *
 * Fill structure for 'type i' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_i(struct s_mips_decode_imm *opcode,
			      unsigned char *buf)
{
  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->rt = (converted >> 16) & 0x1F;
     opcode->im = (converted >> 0) & 0xFFFF;
  } else {
     printf("[CONV_I] Where am I ?!?!?!\n");
     exit(-1);
  }

}

/**
 * @fn void    mips_convert_format_j(struct s_mips_decode_jump *opcode, unsigned char *buf)
 * @brief Convert 'type j' instruction.
 *
 * Fill structure for 'type j' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_j(struct s_mips_decode_jump *opcode,
			      unsigned char *buf)
{

  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->ta = (converted >> 0) & 0x3FFFFFF;
  } else {
     printf("[CONV_J] Where am I ?!?!?!\n");
     exit(-1);
  }

}

/**
 * @fn void    mips_convert_format_t(struct s_mips_decode_trap *opcode, unsigned char *buf)
 * @brief Convert 'trap' instruction.
 * 
 * Fill structure for 'trap' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_t(struct s_mips_decode_trap *opcode,
			      unsigned char *buf)
{
  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
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

/**
 * @fn void    mips_convert_format_cop2(struct s_mips_decode_cop2 *opcode, unsigned char *buf)
 * @brief Convert instrction from second coprocesor
 *
 * Fill structure for 'second coprocesor' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_cop2(struct s_mips_decode_cop2 *opcode,
			      unsigned char *buf)
{
  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->rt = (converted >> 16) & 0x1F;
     opcode->rd = (converted >> 11) & 0x1F;
     opcode->fn = (converted >> 3) & 0xFF;
     opcode->sl = (converted >> 0) & 0x7;
  } else {
     printf("[CONV_COP2] Where am I ?!?!?!\n");
     exit(-1);
  }

}

/**
 * @fn void    mips_convert_format_mov(struct s_mips_decode_mov *opcode, unsigned char *buf)
 * @brief Convert *mov* instruction.
 *
 * Fill structure for 'mov' MIPS instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_mov(struct s_mips_decode_mov *opcode,
			      unsigned char *buf)
{
  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op = (converted >> 26) & 0x3F;
     opcode->rs = (converted >> 21) & 0x1F;
     opcode->cc = (converted >> 18) & 0x7;
     opcode->b1 = (converted >> 17) & 0x1;
     opcode->tf = (converted >> 16) & 0x1;
     opcode->rd = (converted >> 11) & 0x1F;
     opcode->b2 = (converted >> 6) & 0x1F;
     opcode->fn = (converted >> 0) & 0x3F;
  } else {
     printf("[CONV_MOV] Where am I ?!?!?!\n");
     exit(-1);
  }

}

/**
 * @fn void    mips_convert_format_cop1x(struct s_mips_decode_mov *opcode, unsigned char *buf)
 * @brief Convert COP1X instruction.
 *
 * Fill structure for MIPS COP1X instruction.
 *
 * @param opcode Pointer to structure which will be fill.
 * @param buf Pointer to data -> instruction.
 */
void	mips_convert_format_cop1x(struct s_mips_decode_cop1x *opcode,
			      unsigned char *buf)
{
  uint32_t	converted;

  if (asm_config_get_endian() == CONFIG_ASM_BIG_ENDIAN) {
     memcpy(opcode, buf, 4);
  } else if (asm_config_get_endian() == CONFIG_ASM_LITTLE_ENDIAN) {
     memcpy(&converted, buf, 4);
     opcode->op  = (converted >> 26) & 0x3F;
     opcode->bs  = (converted >> 21) & 0x1F;
     opcode->in  = (converted >> 16) & 0x1F;
     opcode->f1  = (converted >> 11) & 0x1F;
     opcode->f2  = (converted >> 6) & 0x1F;
     opcode->fn  = (converted >> 3) & 0x3;
     opcode->fmt = (converted >> 0) & 0x3;
  } else {
     printf("[CONV_MOV] Where am I ?!?!?!\n");
     exit(-1);
  }

}
