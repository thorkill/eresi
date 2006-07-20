
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: elfThor.c,v 1.4 2006-07-20 17:27:09 thor Exp $
 *
 */

#include <libmjollnir.h>

void usage() {
 printf("./elfThor [-AR -o <outfile> -d <symbol> -r <old_symbol:new_symbol> ] -i <infile>\n\
 -A\t- perform analyse\n\
 -R\t- rebuild symtab\n\
 -d\t- delete symbol\n\
 -r\t- rename symbol\n\
 -o\t- output file\n\
 ");
}

int main(int ac, char **av) {
 mjrSession sess;
 char *infile,*outfile, *delsym, *rensym;
 int opt_R,opt_A,nr;

 opt_R=opt_A=0;
 infile = outfile = delsym = rensym = NULL;

 while ((nr=getopt(ac, av, "i:o:ARd:r:"))!=-1) {
   switch(nr) {
    case 'i':
     infile = optarg;
    break;
    case 'A':
     opt_A = 1;
     break;
    case 'R':
     opt_R = 1;
     opt_A = 1;
     break;
    case 'o':
     outfile = optarg;
    break;
    case 'd':
     delsym = optarg;
    break;
    case 'r':
     rensym = optarg;
    break;
    default:
	 usage();
	 return 1;
    break;
   }
 }

 if (!infile) {
  usage();
  return 1;
 }

 if (!mjr_init_session(&sess)) {
  printf("mjrInitSession faild.\n");
  exit(1);
  }

 sess.obj = elfsh_map_obj(infile);

 if (sess.obj == NULL) {
  printf("elfsh_map_obj faild.\n");
  exit(1);
 }

 mjr_setup_processor(&sess);

 if (opt_A) {
  mjr_analyse(&sess,NULL);
 }

 if (opt_R) {
  mjr_symtab_rebuild(&sess);
 }

/* just for tests */
 if (delsym) {
  if (mjr_symbol_delete_by_name(&sess,delsym))
   printf("deleted %s\n",delsym);
 }

 if (rensym) {
  char *o,*n,*brk;
  o = strtok_r(rensym, ":", &brk);
  n = strtok_r(NULL, ":", &brk);
  printf("Rename %s -> %s\n", o, n);
  mjr_symbol_rename(&sess,o,n);
 }

 if (outfile)
     elfsh_save_obj(sess.obj,outfile);

 return 0;
}
