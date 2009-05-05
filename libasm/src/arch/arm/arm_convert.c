#include <libasm.h>

// TODO: put the function prototypes in libasm-arm.h

void	arm_convert_dataproc(struct s_arm_decode_dataproc *opcode, u_char *buf)
{

#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x03;
  opcode->i = (converted >> 25) & 0x01;
  opcode->op2 = (converted >> 21) & 0x0F;
  opcode->s = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->shifter_operand = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = (opcode->shifter_operand & 0x0F);
  opcode->rs = opcode->rotate_imm = ((opcode->shifter_operand >> 8) & 0x0F);
  opcode->immed = (opcode->shifter_operand) & 0x0F;
  opcode->shift = ((opcode->shifter_operand >> 5) & 0x03);
  opcode->shift_imm = ((opcode->shifter_operand >> 7) & 0x1F);
  opcode->reg_shift = ((opcode->shifter_operand >> 4) & 0x01);
}

void	arm_convert_multiply(struct s_arm_decode_multiply *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 24) & 0x03;
  opcode->op = (converted >> 21) & 0x01;
  opcode->s = (converted >> 20) & 0x01;
  opcode->r1 = (converted >> 16) & 0x0F;
  opcode->r2 = (converted >> 12) & 0x0F;
  opcode->r3 = (converted >> 8) & 0x0F;
  opcode->none2 = (converted >> 7) & 0x01;
  opcode->y = (converted >> 6) & 0x01;
  opcode->x = (converted >> 5) & 0x01;
  opcode->none3 = (converted >> 4) & 0x01;
  opcode->r4 = (converted & 0x0F);

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_ldst(struct s_arm_decode_ldst *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x03;
  opcode->reg_offset = (converted >> 25) & 0x01;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->b = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->imm_offset = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = opcode->imm_offset & 0x0F;
  opcode->shift_imm = (opcode->imm_offset >> 7) & 0x1F;
  opcode->shift = (opcode->imm_offset >> 5) & 0x03;
}

void	arm_convert_ldst_misc(struct s_arm_decode_ldst_misc *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->i = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->immedH = (converted >> 8) & 0x0F;
  opcode->none1 = (converted >> 7) & 0x01;
  opcode->s = (converted >> 6) & 0x01;
  opcode->h = (converted >> 5) & 0x01;
  opcode->none2 = (converted >> 4) & 0x01;
  opcode->immedL_rm = converted & 0x0F;
#else
  memcpy(opcode,buf,4);
#endif

}

void	arm_convert_ldst_mult(struct s_arm_decode_ldst_mult *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->s = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->reg_list = converted & 0xFFFF;

#else
  memcpy(opcode,buf,4);
#endif
}

void    arm_convert_branch1(struct s_arm_decode_branch1 *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->l_h = (converted >> 24) & 0x01;
  opcode->signed_imm = converted & 0x00FFFFFF;

#else
  memcpy(opcode,buf,4);
#endif
}

void    arm_convert_branch2(struct s_arm_decode_branch2 *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none1 = (converted >> 6) & 0x03FFFFF;
  opcode->op = (converted >> 5) & 0x01;
  opcode->none2 = (converted >> 4) & 0x01;
  opcode->rm = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}
