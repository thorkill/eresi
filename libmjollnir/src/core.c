/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: core.c,v 1.10 2006-12-11 13:57:39 may Exp $
 *
 */
#include "libmjollnir.h"



/* Main analysis function */
int		mjr_analyse(mjrsession_t *sess,int flags) 
{
 char		*shtName;
 elfsh_Shdr	*shtlist, *shdr;
 elfsh_Sym	*sym;
 int		num_sht,idx_sht;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 if ((NULL == sess) || (NULL == sess->cur))
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid NULL parameters", -1);
 shtlist = elfsh_get_sht(sess->cur->obj, &num_sht);
 if (!shtlist)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed to get SHT", -1);
 
#if __DEBUG_MJOLLNIR__
 fprintf(D_DESC,"[__DEBUG__] mjr_analize: Found %d sections.\n",num_sht);
#endif
 
 /* Analyse all executable sections */
 for (idx_sht = 0; idx_sht < num_sht; idx_sht++) 
   {
     shdr    = (shtlist + idx_sht);
     sym     = elfsh_get_sym_from_shtentry(sess->cur->obj, shdr);
     shtName = elfsh_get_symbol_name(sess->cur->obj, sym);
     
     if (elfsh_get_section_execflag(shdr)) 
       {
#if 1 //__DEBUG_MJOLLNIR__
	 fprintf(D_DESC, 
		 "[__DEBUG__] mjr_analize: Executable section name=(%14s) "
		 "index=(%02i)\n", shtName, idx_sht);
#endif
	 mjr_find_calls(sess,shtName);
       }
   }

 /* Set the flag and return */
 sess->cur->analysed_calls = 1;
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* This function will find calls including calls trought a pointer */
int		 mjr_find_calls(mjrsession_t *sess,char *section_name) 
{
  
  elfshsect_t    *sct;
  asm_instr       instr;
  unsigned char  *ptr;
  unsigned long   curr,len;
  u_int           vaddr, ilen;
  int		  dest;
  char 		  *tmp,*md5;
  mjrblock_t	  *newBlock;
  mjrfunction_t	  *fun;

#if __DEBUG_MJOLLNIR__
 char *_d_type;
#endif

 ELFSH_NOPROFILE_IN();

 /* load section */
#if __DEBUG_MJOLLNIR__
 fprintf(D_DESC, "[__DEBUG_MJOLLNIR__] mjrFindCalls: loading %s\n",section_name);
#endif

 sct = elfsh_get_section_by_name(sess->cur->obj, section_name, NULL, NULL, NULL);

 if (!sct)
   ELFSH_NOPROFILE_ROUT(NULL);

#if __DEBUG_CALLS__
 fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: %x %d %d\n",
	 sct->shdr->sh_addr,
	 sct->shdr->sh_size,
	 sct->shdr->sh_offset);
#endif

 /* allocate memory for in-memory copy */
 XALLOC(ptr,sct->shdr->sh_size, NULL);
 memset(ptr, 0, sct->shdr->sh_size);

 /* load it */
 elfsh_raw_read(sess->cur->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);
 len      = sct->shdr->sh_size;
 curr     = 0;
 vaddr    = sct->shdr->sh_addr;
 newBlock = mjr_create_block(vaddr, section_name, MJR_TYPE_SECT_START);
 tmp      = _vaddr2string(vaddr);
 hash_add(&sess->cur->blocks,tmp,newBlock);

 /* Read all instructions of the section */
 while (curr < len) 
   {
     ilen = asm_read_instr(&instr, ptr + curr, len - curr, &sess->cur->proc);
     if (ilen > 0) 
       {
	 mjr_history_update(sess, instr);
	 sess->cur->curVaddr = vaddr + curr;
	 dest = 0;

#if __DEBUG_READ__
	 fprintf(D_DESC, "[__DEBUG_READ__] mjrFindCalls: v:%x c:%x l:%x il:%x "
		 "%x/%x\n", vaddr + curr, curr, len, ilen,
		 sct->shdr->sh_offset + sct->shdr->sh_size,
		 sct->shdr->sh_offset + curr);
#endif
	 
	 switch (instr.type) 
	   {
	     
	   case ASM_TYPE_CALLPROC:
#if __DEBUG_CALLS__
	     fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: CALL v:0x%lx\n", 
		     vaddr + curr);
#endif
	     sess->cur->st_calls_seen++;
	     if (mjr_get_call_dst(sess, &dest) > 0) 
	       {
		 dest += curr + ilen;
		 if (vaddr + dest != 0x00) 
		   {
		     
		     sess->cur->st_calls_found++;
		     tmp = _vaddr2string(vaddr+dest);
		     if (hash_get(&sess->cur->blocks,tmp) == NULL) 
		       {
#if __DEBUG_MJOLLNIR__
			 _d_type = "NEW";
#endif
			 newBlock = mjr_create_block(vaddr + dest, section_name,
						     MJR_TYPE_FUNCT);
			 fun = mjr_function_create(vaddr + dest);
			 hash_add(&sess->cur->blocks, tmp, newBlock);
			 md5 = mjr_fingerprint_function(sess, ptr + dest, 
							MJR_FNG_TYPE_MD5);
			 if (md5)
			   fun->md5 = elfsh_strdup(md5);
		       } 
#if __DEBUG_MJOLLNIR__
		     else 
		       _d_type = "OLD";
		     fprintf(D_DESC, 
			     " [DM] Found %s CALL at v:0x%x -> [0x%08x] \n",
			     _d_type,sess->cur->curVaddr,vaddr+dest);
#endif
		   }
	       }
	     break;
	     
	   default:
	     break;
	   } 
       } 
     
     /* check if disassembler is missing something */
     if (ilen <= 0) 
       {
	 fprintf(D_DESC, "asm_read_instr error: 0x%lx: %d\n", vaddr + curr, ilen);
	 fprintf(D_DESC, ";; dumping opcodes: %02x %02x %02x %02x\n",
		 *(ptr + curr), *(ptr + curr + 1),
		 *(ptr + curr + 2), *(ptr + curr + 3));
	 ilen = 1;
       }
     curr += ilen;
   }
 
 elfsh_free(ptr);
 ELFSH_NOPROFILE_ROUT(NULL);
}
