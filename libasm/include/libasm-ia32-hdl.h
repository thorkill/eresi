#ifndef _LIBASM_IA32_HDL_H_
#define _LIBASM_IA32_HDL_H_

/**
 * Initializing functions.
 */

int	asm_register_ia32();
int	asm_register_ia32_opcodes();
int	asm_register_ia32_operands();

/**
 * Operand fetching handlers.
 */
int	asm_operand_fetch(asm_operand *operand, u_char *opcode, int type, 
			  asm_instr *ins);
int	asm_operand_fetch_default(asm_operand *operand, u_char *opcode, int type, 
				  asm_instr *ins);
int	asm_operand_fetch_opmod(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_encoded(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_encodedbyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_general(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_generalbyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_jump(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_shortjump(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_offset(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_fixed(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_fpu(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediate(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediatebyte(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_address(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_register(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_control(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_debug(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_xsrc(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_ydest(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_pmmx(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_memory(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_immediateword(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);
int	asm_operand_fetch_segment(asm_operand *operand, u_char *opcode, int type,
				asm_instr *ins);

#endif
