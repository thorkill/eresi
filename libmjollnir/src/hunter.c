
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: hunter.c,v 1.1.1.4 2006-03-15 21:46:43 thor Exp $
 *
 */

#include <libmjollnir.h>

/*
 * search for call, take dst, prepare temporary name insert vaddr and name
 */

int
mjr_find_calls_raw(
		   Mjr_CTX * ctx,
		   hash_t * hs,
		   char *section,
		   char *prefix
)
{
	elfshsect_t    *sct;
	unsigned char  *ptr;
	int		vaddr     , dest, ilen, n;
	unsigned long	curr, len;
	asm_instr	instr;
	char		buff      [BSIZE];
	char           *md5sum;
	Mjr_fsym       *fl;

	n = 0;

	hash_init(hs, MJR_MAX_FNG);
	sct = elfsh_get_section_by_name(ctx->obj, section, NULL, NULL, NULL);
	if (!sct) {
		return -1;
	}
#if __DEBUG__
	fprintf(D_DESC, "mjr_find_calls_raw: %x %d %d\n",
		sct->shdr->sh_addr,
		sct->shdr->sh_size,
		sct->shdr->sh_offset);
#endif

	ptr = elfsh_malloc(sct->shdr->sh_size);
	memset(ptr, 0, sct->shdr->sh_size);

	elfsh_raw_read(ctx->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);

	len = sct->shdr->sh_size;
	curr = 0;
	vaddr = sct->shdr->sh_addr;

	while (curr < len) {

		if ((ilen = asm_read_instr(&instr, ptr + curr, len - curr, &ctx->proc)) > 0) {
			mjr_ihist(ctx, instr);
			ctx->cvaddr = vaddr + curr;
			dest = 0;

#if __DEBUG_READ__
			fprintf(D_DESC, "mjr_find_calls_raw: curr: %x %x/%x %x %x/%x\n",
				vaddr + curr, curr, len,
				ilen,
				sct->shdr->sh_offset + sct->shdr->sh_size,
				sct->shdr->sh_offset + curr);
#endif

			switch (instr.type) {

			case ASM_TYPE_CALLPROC:
				ctx->stats_calls_seen++;
				if (mjr_call_dest(ctx, &dest) > 0) {
					dest += curr + ilen;
					if (vaddr + dest != 0x00) {

						memset(buff, 0, BSIZE);
						snprintf(buff, BSIZE - 1, "%s%x", prefix, dest);

						ctx->stats_calls_found++;

						if (hash_get(hs, buff) == NULL) {

							md5sum = mjr_fingerprint_function(ctx, ptr + dest, FNG_TYPE_MD5, 30);
							if (md5sum != NULL) {

								fl = elfsh_malloc(sizeof(Mjr_fsym));
								fl->vaddr = vaddr + dest;
								fl->epoint = vaddr + curr;
								fl->name = strdup(buff);
								fl->type = F_TYPE_CALL;
								fl->md5sum = strdup(md5sum);
								hash_add(hs, fl->name, fl);
							}
#if __DEBUG__
							fprintf(D_DESC, "mjr_find_calls_raw: vaddr:0x%x call:0x%x md5:%s\n",
								fl->epoint,
							 fl->vaddr, md5sum);
#endif
						}
					}
				}
				break;

			case ASM_TYPE_CONDBRANCH:
#if __DEBUG__
				fprintf(D_DESC, "mjr_find_calls_raw: %x ASM_TYPE_CONDBRANCH not supported\n",
					ctx->cvaddr);
#endif
				break;

			default:
				/* syscall stuff */
				if (
				    (instr.instr == ASM_INT)
				    && (instr.op1.content & ASM_OP_VALUE)
				    && (ctx->ihist[1].instr == ASM_MOV)
				    && (ctx->ihist[1].op2.content == ASM_OP_VALUE)
					) {
					u_int		val;
					asm_operand_get_immediate(&instr, 1, 0, &dest);
					asm_operand_get_immediate(&ctx->ihist[1], 2, 0, &val);

					if (dest != 0x80) {
						continue;
					}
					memset(buff, 0, BSIZE);
					snprintf(buff, BSIZE - 1, "__sys_call_%d", val);

					if (hash_get(hs, buff) == NULL) {
						fl = elfsh_malloc(sizeof(Mjr_fsym));

						fl->name = strdup(buff);
						fl->type = F_TYPE_SYSCALL;
						fl->vaddr = (vaddr + curr) - ctx->ihist[1].len;
						fl->epoint = val;
						fl->md5sum = NULL;

#if __DEBUG__
						fprintf(D_DESC, "mjr_find_calls_raw: syscall found at %lx %x %d\n",
							(vaddr + curr) - ctx->ihist[1].len,
							dest, val);
#endif

						hash_add(hs, fl->name, fl);
					}
				}
				break;
			}	/* end of switch */
		}
		if (ilen <= 0) {
			fprintf(D_DESC, "asm_read_instr error: 0x%lx: %d\n", vaddr + curr, ilen);
			fprintf(D_DESC, ";; dumping opcodes: %02x %02x %02x %02x\n",
				*(ptr + curr), *(ptr + curr + 1),
				*(ptr + curr + 2), *(ptr + curr + 3));
			ilen = 1;
		}
		curr += ilen;
	}

	elfsh_free(ptr);
	return 0;
}


/*
 * search for call using symtab
 */

int
mjr_find_calls_symtab(
		      Mjr_CTX * ctx,
		      hash_t * hs
)
{
	elfshsect_t    *sct, *sct2;
	elfsh_Sym      *symtab;
	elfsh_Sym      *table;

	unsigned char  *ptr;
	int		vaddr     , dest, foff, n, num, index;
	unsigned long	curr;
	char           *md5sum;
	Mjr_fsym       *fl;

	n = 0;

	hash_init(hs, MJR_MAX_FNG);

	symtab = elfsh_get_symtab(ctx->obj, &num);
	sct2 = elfsh_get_section_by_type(ctx->obj, SHT_SYMTAB, NULL, NULL, NULL, 0);
	sct = elfsh_get_section_by_name(ctx->obj, ".text", NULL, NULL, NULL);

	vaddr = sct->shdr->sh_size;
	foff = sct->shdr->sh_offset;

	table = (elfsh_Sym *) elfsh_get_raw(sct2);

	ptr = elfsh_malloc(sct->shdr->sh_size);
	memset(ptr, 0, sct->shdr->sh_size);
	elfsh_raw_read(ctx->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);

	for (index = 0; index < num; index++) {
		u_int		type;
		type = elfsh_get_symbol_type(table + index);

		if (type == STT_FUNC) {
			dest = elfsh_get_symbol_foffset(ctx->obj, table + index);
			if (dest == 0) {
				curr = dest;
			} else {
				curr = dest - foff;
			}
			md5sum = mjr_fingerprint_function(ctx, ptr + curr, FNG_TYPE_MD5, 30);
			if (md5sum != NULL) {
				fl = elfsh_malloc(sizeof(Mjr_fsym));
				fl->vaddr = dest;
				fl->type = F_TYPE_CALL;
				fl->name = strdup(elfsh_get_symbol_name(ctx->obj, table + index));
				fl->md5sum = strdup(md5sum);
				hash_add(hs, fl->name, fl);
				n++;
			}
		}
	}

	elfsh_free(ptr);
	return 0;
}
