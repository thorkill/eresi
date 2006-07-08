
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: core.c,v 1.1 2006-07-08 21:24:19 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjrAnalize(mjrSession *sess,int flags) {
 char *shtName;
 elfsh_Shdr *shtlist,*shdr;
 elfsh_Sym  *sym;
 int num_sht,idx_sht;

 if (!(shtlist = elfsh_get_sht(sess->obj, &num_sht))) {
  printf("faild to get sht!\n");
  return -1;
 }

#if __DEBUG__
 fprintf(D_DESC,"[__DEBUG__] mjrAnalize: Found %d sections.\n",num_sht);
#endif
					 
 for (idx_sht = 0; idx_sht < num_sht; idx_sht++) {
  shdr = (shtlist + idx_sht);
  sym = elfsh_get_sym_from_shtentry(sess->obj, shdr);
  shtName = elfsh_get_symbol_name(sess->obj, sym);
							
  if (elfsh_get_section_execflag(shdr)) {
#if __DEBUG__
   fprintf(D_DESC,"[__DEBUG__] mjrAnalize: Executable section name=(%14s) index=(%02i)\n", shtName, idx_sht);
#endif
   mjrFindCalls(sess,shtName);
  }
 }
  
 return NULL;
}

/*
 This function will find calls including
 calls trought a pointer
 */

int mjrFindCalls(mjrSession *sess,char *section_name) {

 elfshsect_t    *sct;
 asm_instr       instr;
 unsigned char  *ptr;
 u_int           vaddr, ilen, dest;
 unsigned long   curr,len;
 char 			*tmp;

 mjrBlock		*newBlock;

#if __DEBUG__
 char *_d_type;
#endif

 ELFSH_NOPROFILE_IN();

 /* load section */
#if __DEBUG__
 fprintf(D_DESC, "[__DEBUG__] mjrFindCalls: loading %s\n",section_name);
#endif

 sct = elfsh_get_section_by_name(sess->obj, section_name, NULL, NULL, NULL);

 if (!sct) {
  ELFSH_NOPROFILE_ROUT(NULL);
 }

#if __DEBUG_CALLS__
 fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: %x %d %d\n",
  sct->shdr->sh_addr,
  sct->shdr->sh_size,
  sct->shdr->sh_offset);
#endif

 /* allocate memory for in-memory copy */
 XALLOC(ptr,sct->shdr->sh_size,-1);
 memset(ptr, 0, sct->shdr->sh_size);

 /* load it */
 elfsh_raw_read(sess->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);

 len = sct->shdr->sh_size;
 curr = 0;
 vaddr = sct->shdr->sh_addr;
 
 newBlock = mjrCreateBlock(vaddr,section_name,MJR_TYPE_SECT_START);
 tmp = _vaddr2string(vaddr);
 hash_add(&sess->blocks,tmp,newBlock);

 while (curr < len) {

  if ((ilen = asm_read_instr(&instr, ptr + curr, len - curr, &sess->proc)) > 0) {
   mjrHistoryUpdate(sess, instr);
   sess->curVaddr = vaddr + curr;
   dest = 0;

#if __DEBUG_READ__
   fprintf(D_DESC, "[__DEBUG_READ__] mjrFindCalls: v:%x c:%x l:%x il:%x %x/%x\n",
    vaddr + curr, curr, len,
    ilen,
    sct->shdr->sh_offset + sct->shdr->sh_size,
    sct->shdr->sh_offset + curr);
#endif
   switch (instr.type) {

    case ASM_TYPE_CALLPROC:
#if __DEBUG_CALLS__
     fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: CALL v:0x%lx\n", vaddr + curr);
#endif

     if (mjrGetCallDst(sess,&dest)>0) {
	  dest += curr + ilen;
	  if (vaddr + dest != 0x00) {
	   tmp = _vaddr2string(vaddr+dest);
	   if (hash_get(&sess->blocks,tmp) == NULL) {
#if __DEBUG__
		_d_type = "NEW";
#endif	    
		newBlock = mjrCreateBlock(vaddr+dest,section_name,MJR_TYPE_FUNCT);
		hash_add(&sess->blocks,tmp,newBlock);
	   } else {
#if __DEBUG__
		_d_type = "OLD";
#endif	    
	   }

#if __DEBUG__
	 fprintf(D_DESC, "[__DEBUG__] Found %s CALL at v:0x%x -> [0x%08x] \n",
	  _d_type,sess->curVaddr,vaddr+dest);
#endif
	  }
     }
	 break;

	default:
	 break;

   /* end of switch */
   }

  /* end of if asm_read_instr */
  }

  /* check if disassembler is missing something */
  if (ilen <= 0) {
   fprintf(D_DESC, "asm_read_instr error: 0x%lx: %d\n", vaddr + curr, ilen);
   fprintf(D_DESC, ";; dumping opcodes: %02x %02x %02x %02x\n",
              *(ptr + curr), *(ptr + curr + 1),
              *(ptr + curr + 2), *(ptr + curr + 3));
              ilen = 1;
  }
 curr += ilen;
 /* end of while */
 }
 
 elfsh_free(ptr);
 ELFSH_NOPROFILE_ROUT(NULL);
}
