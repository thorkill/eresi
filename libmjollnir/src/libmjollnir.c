
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir.c,v 1.1.1.4 2006-03-15 21:46:43 thor Exp $
 */

#include <libmjollnir.h>

/*
 * this function 'cuts' and copy the buffer pointed by src into dst, mlen
 * describes the maximal lengh of dst buffer
 */

int
mjr_i386_cut_function(Mjr_CTX * ctx, unsigned char *src, unsigned char *dst, int mlen)
{
	int		n         , ilen, p;
	asm_instr	instr, hist[4];

	p = 0;
	ilen = 0;
	for (n = 0; n < mlen; n += ilen) {
		ilen = asm_read_instr(&instr, src + n, mlen - n, &ctx->proc);
		if (ilen <= 0) {
			return p;
		}
		/* filter this out */
		if (instr.instr != ASM_NOP) {
			dst[p++] = src[n];
		}
		/* epilog */
		if ((hist[0].instr == ASM_LEAVE) && (instr.instr == ASM_RET)) {
			dst[p++] = src[n];
			return p;
		} else if ((hist[0].instr == ASM_POP) && (instr.instr == ASM_RET)) {
			dst[p++] = src[n];
			return p;
		}
		hist[1] = hist[0];
		hist[0] = instr;
	}

	return 0;
}

/**
 * This function inserts new symbol as a function.
 * shortcut for insert/se
 */

int
mjr_add_symbol(elfshobj_t * obj, char *section, u_int vaddr, char *fname)
{

	elfshsect_t    *sct;
	elfsh_Sym	sym;

	sct = elfsh_get_section_by_name(obj, section, NULL, NULL, NULL);
	sym = elfsh_create_symbol(vaddr, 0, STT_FUNC, 0, 0, sct->index);
	elfsh_insert_symbol(obj->secthash[ELFSH_SECTION_SYMTAB], &sym, fname);

	return 0;
}


/*
 * Context ini
 */

int
mjr_init(Mjr_CTX * x)
{
	x->obj = NULL;
	x->md5db = NULL;
	x->stats_calls_seen = 0;
	x->stats_calls_found = 0;
	return 1;
}

/**
 * History manager
 */

int
mjr_ihist(Mjr_CTX * x, asm_instr inst)
{

	if (inst.instr == ASM_NOP) {
		return 1;
	}
	x->ihist[3] = x->ihist[2];
	x->ihist[2] = x->ihist[1];
	x->ihist[1] = x->ihist[0];
	x->ihist[0] = inst;

	return 1;
}

/**
 * This function returns destination of a call
 */

int
mjr_call_dest(Mjr_CTX * ctx, int *dst)
{
	int		ret = 0;

	if (ctx->ihist[0].instr != ASM_CALL) {
		return ret;
	}
#if __DEBUG__
	fprintf(D_DESC, "mjr_call_dest: start - 0x%08x %d\n",
		ctx->cvaddr, ctx->ihist[0].op1.content);
#endif

	if ((ctx->ihist[0].op1.content & ASM_OP_VALUE) &&
	    !(ctx->ihist[0].op1.content & ASM_OP_REFERENCE)
		) {
		if (asm_operand_get_immediate(&ctx->ihist[0], 1, 0, dst) != -1) {
			ret = 1;
		} else {
			fprintf(D_DESC, "mjr_call_dest: get_immediete returned -1!\n");
		}
	} else if ((ctx->ihist[0].op1.content & ASM_OP_ADDRESS) &&
		   ctx->ihist[1].instr == ASM_MOV &&
		   ctx->ihist[2].instr == ASM_MOV &&
		   !(ctx->ihist[2].op1.content & ASM_OP_SCALE) &&
		   !(ctx->ihist[2].op2.content & ASM_OP_REFERENCE) &&
		   ctx->ihist[2].op2.imm != 0x0) {


#if __DEBUG_OPERAND__
		asm_debug_operand(&ctx->ihist[0].op1);
		asm_debug_operand(&ctx->ihist[0].op2);
		asm_debug_operand(&ctx->ihist[2].op1);
		asm_debug_operand(&ctx->ihist[2].op2);
#endif

		dst = (int *)ctx->ihist[2].op2.imm;

#if __DEBUG__
		fprintf(D_DESC, "mjr_call_dest: PTR FUNCTION ! %x %x\n",
			ctx->cvaddr, (int)dst);
#endif
		ret = 1;
	} else {
		fprintf(D_DESC, "mjr_call_dest: not supported! 0x%08x %d\n",
			ctx->cvaddr, ctx->ihist[0].op1.content);
	}

#if __DEBUG__
	fprintf(D_DESC, "mjr_call_dest: vaddr: %x dst: %x\n",
		ctx->cvaddr,
		*dst);
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
