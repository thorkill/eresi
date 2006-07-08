
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: destCall.c,v 1.1 2006-07-08 21:24:21 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjrGetCallDst(mjrSession *sess,int *dest) {
	int		ret = 0;

	if (sess->ihist[0].instr != ASM_CALL) {
		return ret;
	}

#if __DEBUG_CALLS__
	fprintf(D_DESC, "[__DEBUG_CALLS__] mjrGetCallDst: start - 0x%08x %d\n",
		sess->curVaddr, sess->ihist[0].op1.content);
#endif

	if ((sess->ihist[0].op1.content & ASM_OP_VALUE) &&
	    !(sess->ihist[0].op1.content & ASM_OP_REFERENCE)
		) {
		if (asm_operand_get_immediate(&sess->ihist[0], 1, 0, dest) != -1) {
			ret = 1;
		} else {
			fprintf(D_DESC, "[NOTICE] mjrGetCallDst: get_immediete returned -1!\n");
		}
	} else if ((sess->ihist[0].op1.content & ASM_OP_ADDRESS) &&
		   sess->ihist[1].instr == ASM_MOV &&
		   sess->ihist[2].instr == ASM_MOV &&
		   !(sess->ihist[2].op1.content & ASM_OP_SCALE) &&
		   !(sess->ihist[2].op2.content & ASM_OP_REFERENCE) &&
		   sess->ihist[2].op2.imm != 0x0) {

#if __DEBUG_OPERAND__
		asm_debug_operand(&sess->ihist[0].op1);
		asm_debug_operand(&sess->ihist[0].op2);
		asm_debug_operand(&sess->ihist[2].op1);
		asm_debug_operand(&sess->ihist[2].op2);
#endif

		dest = (int *)sess->ihist[2].op2.imm;

#if __DEBUG_PTR__
		fprintf(D_DESC, "[__DEBUG_PTR__] mjrGetCallDst: PTR FUNCTION ! %x %x\n",
			sess->curVaddr, (int)dest);
#endif
		ret = 1;
	} else {
		fprintf(D_DESC, "[NOTICE] mjrGetCallDst: not supported! 0x%08x %d\n",
			sess->curVaddr, sess->ihist[0].op1.content);
	}

#if __DEBUG_CALLS__
	fprintf(D_DESC, "[__DEBUG_CALLS__] mjrGetCallDst: vaddr: %x dest: %x\n",
		sess->curVaddr,
		*dest);
#endif

	return ret;
}

int
asm_debug_operand(asm_operand * op)
{
	fprintf(D_DESC, "[*] Operand Dump\n[*] len: %d type: %d size: %d content: %d\n",
		op->len, op->type, op->size, op->content
	);

	fprintf(D_DESC, "[*] Content: %s %s %s %s %s %s %s\n",
		(op->content & ASM_OP_VALUE) ? "ASM_OP_VALUE" : ".",
		(op->content & ASM_OP_BASE) ? "ASM_OP_BASE" : ".",
		(op->content & ASM_OP_INDEX) ? "ASM_OP_INDEX" : ".",
		(op->content & ASM_OP_SCALE) ? "ASM_OP_SCALE" : ".",
		(op->content & ASM_OP_FIXED) ? "ASM_OP_FIXED" : ".",
		(op->content & ASM_OP_REFERENCE) ? "ASM_OP_REFERENCE" : ".",
		(op->content & ASM_OP_ADDRESS) ? "ASM_OP_ADDRESS" : ".");

	return 0;
}
