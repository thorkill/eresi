
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: hunter.c,v 1.1.1.2 2006-02-23 22:42:25 thor Exp $
 */

#include <libmjollnir.h>

/*
 search for call, take dst, prepare temporary name
 insert vaddr and name
 */

int mjr_find_calls_raw(
 Mjr_CTX *ctx,
 hash_t	*hs,
 char *section,
 char *prefix
 ) {
  elfshsect_t 	*sct;
  unsigned char 			*ptr;
  int 			vaddr,dest,ilen,n;
  unsigned long curr,len;
  asm_processor proc;
  asm_instr     instr;
  char			buff[BSIZE];
  char			*md5sum;
  Mjr_fsym		*fl;

  n = 0;

  hash_init(hs,MJR_MAX_FNG);
  sct = elfsh_get_section_by_name(ctx->obj, section, NULL, NULL, NULL);

#if __DEBUG__
  fprintf(stderr,"%x %d %d\n", sct->shdr->sh_addr, sct->shdr->sh_size, sct->shdr->sh_offset);
#endif

  ptr = elfsh_malloc(sct->shdr->sh_size);
  memset(ptr,0,sct->shdr->sh_size);

  elfsh_raw_read(ctx->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);
  asm_init_i386(&proc);

  len = sct->shdr->sh_size;
  curr = 0;
  dest = 0;
  vaddr = sct->shdr->sh_addr;

  while(curr < len) {

    if ((ilen = asm_read_instr(&instr, ptr + curr, len - curr, &proc))>0) {

#if __DEBUG__
	 fprintf(stderr,"curr: %x %x/%x %x %x/%x\n",vaddr+curr,curr,len,ilen,sct->shdr->sh_offset+ sct->shdr->sh_size,sct->shdr->sh_offset+curr);
#endif

	  if ((instr.instr == ASM_CALL) && 
	      (instr.op1.content & ASM_OP_VALUE) &&
		  !(instr.op1.content & ASM_OP_REFERENCE)
		  ) {
		   asm_operand_get_immediate(&instr, 1, 0, &dest);
		   dest += curr + ilen;
		   if (vaddr+dest != 0x00) {


		    memset(buff,0,BSIZE);
		    snprintf(buff,BSIZE-1,"%s%x",prefix,dest);

			  if (hash_get(hs,buff) == NULL) {
               fl = elfsh_malloc(sizeof(Mjr_fsym));

  			   md5sum = mjr_fingerprint_function(ptr+dest,FNG_TYPE_MD5,30);

		       fl->vaddr = vaddr + dest;
			   fl->epoint = vaddr + curr;
		       fl->name = strdup(buff);
			   fl->type = F_TYPE_CALL;
			   fl->md5sum = strdup(md5sum);
			   hash_add(hs,fl->name,fl);
#if __DEBUG__
		    fprintf(stderr,"vaddr:0x%lx call:0x%x md5:%s\n", fl->epoint, fl->vaddr,md5sum);
#endif
			  }
			 }

			/* syscall stuff */
			} else if (
			     (instr.instr == ASM_INT) 
			  && (instr.op1.content & ASM_OP_VALUE)
			  && (ctx->ihist[0].instr == ASM_MOV)
			  && (ctx->ihist[0].op2.content == ASM_OP_VALUE)
			 ) {
			 u_int val;
			 asm_operand_get_immediate(&instr, 1, 0, &dest);
			 asm_operand_get_immediate(&ctx->ihist[0], 2, 0, &val);

			 if (dest != 0x80) {continue;}

			 
			 memset(buff,0,BSIZE);
			 snprintf(buff,BSIZE-1,"__sys_call_%d",val);

             if (hash_get(hs,buff) == NULL) {

			  fl = elfsh_malloc(sizeof(Mjr_fsym));

			  fl->name = strdup(buff);
			  fl->type = F_TYPE_SYSCALL;
			  fl->vaddr = (vaddr + curr) - ctx->ihist[0].len;
			  fl->epoint = val;
			  fl->md5sum = NULL;

#if __DEBUG__
			 fprintf(stderr,"syscall found at %x %x %d \n", (vaddr + curr) - ctx->ihist[0].len, dest, val);
#endif

			  hash_add(hs,fl->name,fl);
			 }
			}
	}

	if (ilen <=0) {
	  fprintf(stderr,"asm_read_instr error: 0x%lx: %d\n",vaddr+curr,ilen);
      fprintf(stderr,";; dumping opcodes: %02x %02x %02x %02x\n",
        *(ptr + curr), *(ptr + curr + 1),
        *(ptr + curr + 2), *(ptr + curr + 3));
	  ilen = 1;
	 }
	 
   mjr_ihist(ctx,instr);

   curr += ilen;
  }

  elfsh_free(ptr);
  return 0;
}


/*
 search for call using symtab
 */

int mjr_find_calls_symtab(
 elfshobj_t *obj,
 hash_t	*hs
 ) {
  elfshsect_t 	*sct,*sct2;
  elfsh_Sym		*symtab;
  elfsh_Sym     *table;

  unsigned char *ptr;
  int 			vaddr,dest,foff,n,num,index;
  unsigned long curr;
  char			*md5sum;
  Mjr_fsym		*fl;

  n = 0;

  hash_init(hs,MJR_MAX_FNG);
  
  symtab = elfsh_get_symtab(obj, &num);
  sct2 = elfsh_get_section_by_type(obj, SHT_SYMTAB, NULL, NULL, NULL, 0);
  sct = elfsh_get_section_by_name(obj, ".text", NULL, NULL, NULL);

  vaddr = sct->shdr->sh_size;
  foff = sct->shdr->sh_offset;
   
  table = (elfsh_Sym *) elfsh_get_raw(sct2);

  ptr = malloc(sct->shdr->sh_size);
  memset(ptr,0,sct->shdr->sh_size);
  elfsh_raw_read(obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);

  for (index = 0; index < num; index++) {
   u_int type;
   type = elfsh_get_symbol_type(table + index);

   if (type == STT_FUNC) {
    dest = elfsh_get_symbol_foffset(obj,table+index);
	if (dest == 0) {curr = dest;} else {curr = dest-foff;}
	md5sum = mjr_fingerprint_function(ptr+curr,FNG_TYPE_MD5,30);
	if (md5sum != NULL) {
 	 fl = malloc(sizeof(Mjr_fsym));
     fl->vaddr = dest;
	 fl->type = F_TYPE_CALL;
     fl->name = strdup(elfsh_get_symbol_name(obj,table+index));
     fl->md5sum = strdup(md5sum);
     hash_add(hs,fl->name,fl);
	 n++;
	}
   }
  }

  free(ptr);
  return 0;
}

