#include <libasm.h>

void	arm_convert_dataproc(struct s_arm_decode_dataproc *opcode, u_char *buf)
{

#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x02;
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

void	arm_convert_ldst(struct s_arm_decode_ldst *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x02;
  opcode->reg_offset = (converted >> 25) & 0x01;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->b = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x01;
  opcode->rd = (converted >> 12) & 0x01;
  opcode->imm_offset = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = opcode->imm_offset & 0x0F;
  opcode->shift_imm = (opcode->imm_offset >> 7) & 0x1F;
  opcode->shift = (opcode->imm_offset >> 5) & 0x03;
}
