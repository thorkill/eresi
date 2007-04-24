/**
 * $Id: operand_handlers.c,v 1.2 2007-04-24 16:25:14 strauss Exp $
 * @file operand_handlers.c
 *
 * Handlers for the operand vector.
 * Each handler is associated to an operand type.
 *
 */
#include <libasm.h>
#include <libasm-int.h>


/**
 * Main function, dispatch processing to handler.
 * @param op Pointer to operand to fill
 * @param opcode Pointer to operand data.
 * @param otype Type of operand to fetch : ASM_OTYPE_*
 * @param proc Pointer to processor structure.
 * @return Operand length or -1 on error (should currently never occur)
 */

int	asm_operand_fetch(asm_operand *op, u_char *opcode, int otype, asm_processor *proc)
{
  vector_t	*vec;
  u_int		dim[2];
  int		to_ret;
  int		(*fetch)(asm_operand *, u_char *, int, asm_processor *);
  
  vec = aspect_vector_get("operand");
  dim[0] = LIBASM_VECTOR_IA32;
  dim[1] = otype;
  
  fetch = aspect_vectors_select(vec, dim);
  to_ret = fetch(op, opcode, otype, proc);
  if (to_ret == -1)
    {
      printf("%s:%i Unsupported operand type : %i\n", __FILE__, __LINE__, otype);
    }
    return (to_ret);
}


/**
 *
 *
 */

int	asm_operand_fetch_opmod(asm_operand *operand, u_char *opcode, int type, asm_processor *proc)
{
  struct s_modrm	*modrm;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->type = ASM_OTYPE_OPMOD;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  modrm = (struct s_modrm *) opcode;
  operand->base_reg = modrm->m;
  return (0);
}

/**
 * Fetch ASM_OTYPE_GENERAL operand
 *
 */

int	asm_operand_fetch_general(asm_operand *operand, u_char *opcode, int type, asm_processor *proc)
{
  struct s_modrm	*modrm;

  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  modrm = (struct s_modrm *) opcode;
  operand->base_reg = modrm->r;
  return (operand->len = 0);
}
/**
 * Fetch ASM_OTYPE_GENERAL operand
 *
 */

int	asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, int type, asm_processor *proc)
{
  struct s_modrm	*modrm;

  operand->type = ASM_OTYPE_GENERAL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_R8;
  modrm = (struct s_modrm *) opcode;
  operand->base_reg = modrm->r;
  return (operand->len = 0);
}

/**
 * Fetch ASM_OTYPE_PMMX operand
 *
 *
 */

int	asm_operand_fetch_pmmx(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  int		len;
  operand->type = ASM_OTYPE_PMMX;
  len = operand_rmv(operand, opcode, 4, proc);
  operand->regset = ASM_REGSET_MM;
  return (len);
}

/**
 * Fetch ASM_OTYPE_ENCODED operand
 *
 *
 */

int	asm_operand_fetch_encoded(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_ENCODED;
  return (operand_rmv(operand, opcode, 4, proc));
}

/**
 * Fetch ASM_OTYPE_ENCODED operand
 *
 *
 */

int	asm_operand_fetch_encodedbyte(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_ENCODED;
  return (operand_rmb(operand, opcode, 5, proc));
}

/**
 *
 *
 */

int	asm_operand_fetch_jump(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_JUMP;
  operand->content = ASM_OP_VALUE | ASM_OP_ADDRESS;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 4;
  memcpy(&operand->imm, opcode, 4);
  return (4);
}

/**
 *
 *
 */

int	asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_JUMP;
  operand->content = ASM_OP_VALUE | ASM_OP_ADDRESS;

  operand->len = 1;
  operand->imm = 0;
  if (*(opcode) >= 0x80u)
    memcpy((char *) &operand->imm + 1, "\xff\xff\xff", 3);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}

/**
 *
 *
 */

int	asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_FIXED;
  return (0);
}

/**
 *
 *
 */

int	asm_operand_fetch_offset(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_OFFSET;
  operand->content = ASM_OP_VALUE | ASM_OP_REFERENCE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 4;
  memcpy(&operand->imm, opcode, 4);
  return (4);
}

/**
 *
 *
 */

int	asm_operand_fetch_immediate(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 4;
  memcpy(&operand->imm, opcode, 4);
  return (4);
}

/**
 *
 *
 */

int	asm_operand_fetch_immediateword(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 2;
  memcpy(&operand->imm, opcode, 2);
  return (2);
}

/**
 *
 *
 */

int	asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 1;
  //
  // Add sign support : putting 0xffffff if byte < 0
  //
  if (*opcode >= 0x80)
    memset(&operand->imm, 0xff, 4);
  memcpy(&operand->imm, opcode, 1);
  return (1);
}

/**
 *
 *
 */

int	asm_operand_fetch_address(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_ADDRESS;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 4;
  memcpy(&operand->imm, opcode, 4);
  return (4);
}

/**
 *
 *
 */

int	asm_operand_fetch_register(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_REGISTER;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->base_reg = modrm->m;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (0);
}

/**
 *
 *
 */

int	asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_CONTROL;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_CREG;
  operand->base_reg = modrm->r;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (1);
}

/**
 *
 *
 */

int	asm_operand_fetch_debug(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_DEBUG;
  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_CREG;
  operand->base_reg = modrm->r;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (1);
}

/**
 *
 *
 */

int	asm_operand_fetch_xsrc(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_XSRC;
  operand->content = ASM_OP_BASE | ASM_OP_REFERENCE;
  operand->base_reg = ASM_REG_ESI;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->prefix = ASM_PREFIX_DS;
  return (0);
}

/**
 *
 *
 */

int	asm_operand_fetch_ydest(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  operand->type = ASM_OTYPE_YDEST;
  operand->content = ASM_OP_BASE | ASM_OP_REFERENCE;
  operand->base_reg = ASM_REG_EDI;
  operand->regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->prefix = ASM_PREFIX_ES;
  return (0);
}

/**
 *
 *
 */

int	asm_operand_fetch_memory(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_MEMORY;

//  operand->content = ASM_OP_BASE;
//  operand->regset = ASM_REGSET_R32;
//  operand->base_reg = modrm->r;

//  return (0);

  return operand_rmv(operand, opcode, 4, proc);
}

/**
 *
 *
 */

int	asm_operand_fetch_segment(asm_operand *operand, u_char *opcode, int otype, 
				  asm_processor *proc)
{
  struct s_modrm	*modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_SEGMENT;

  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_SREG;
  operand->base_reg = modrm->r;

  return (0);
}

