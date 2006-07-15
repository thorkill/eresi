
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: init.c,v 1.2 2006-07-15 17:06:07 thor Exp $
 *
 */

#include "libmjollnir.h"

/*
 Init stuff
 */

int mjr_init_session(mjrSession *sess) {


 ELFSH_NOPROFILE_IN();

 sess->obj = NULL;
 sess->curVaddr = 0;
 hash_init(&sess->blocks,mjrHashLarge);

 ELFSH_NOPROFILE_ROUT(1);

}

int mjr_setup_processor(mjrSession *sess) {
 u_int         arch;
 arch =  elfsh_get_arch(sess->obj->hdr);
 switch(arch)
 {
  case EM_SPARC:
//   asm_init_sparc(&sess->proc);
   break;
  case EM_386:
   asm_init_i386(&sess->proc);
   break;
  default:
   printf("unsupported architecture\n");
   return (-1);
 }

 return 1;
}
