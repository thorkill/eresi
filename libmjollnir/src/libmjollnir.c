
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir.c,v 1.1.1.2 2006-02-23 22:35:33 thor Exp $
 */

#include <libmjollnir.h>

/*
 this function 'cuts' and copy the buffer pointed
 by src into dst, mlen describes the maximal lenght 
 of dst buffer
 */

int mjr_i386_cut_function(unsigned char *src,unsigned char *dst, int mlen) {
  int n,ilen,p;
  asm_processor proc;
  asm_instr     instr,hist[4];

  asm_init_i386(&proc);

  p=0;
  ilen=0;
  for (n=0;n<mlen;n+=ilen) {
   ilen = asm_read_instr(&instr, src+n, mlen-n, &proc);
   if (ilen <= 0) {return p;}

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
 * shortcut for insert/set
 */

int mjr_add_symbol(elfshobj_t *obj,char *section,u_int vaddr,char *fname) {

 elfshsect_t	*sct;
 elfsh_Sym		sym;

 sct = elfsh_get_section_by_name(obj, section, NULL, NULL, NULL);
 sym = elfsh_create_symbol(vaddr, 0, STT_FUNC, 0, 0, sct->index);
 elfsh_insert_symbol(obj->secthash[ELFSH_SECTION_SYMTAB], &sym, fname);

 return 0;
}


/*
 Context init
 */

int mjr_init(Mjr_CTX *x) {
  x->obj = NULL;
  x->md5db = NULL;
 return 1;
}

/*
 History manager
 */

int mjr_ihist(Mjr_CTX *x,asm_instr inst) {

 x->ihist[3]=x->ihist[2];
 x->ihist[2]=x->ihist[1];
 x->ihist[1]=x->ihist[0];
 x->ihist[0]=inst;

 return 1;
}
