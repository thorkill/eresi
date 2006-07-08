
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: elfThor.c,v 1.1 2006-07-08 21:24:15 thor Exp $
 *
 */

#include <libmjollnir.h>

void usage() {
 printf("./elfThor [-R -o <outfile>] -i <infile>\n\
 -R\t- rebuild symtab\n\
 -o\t- output file\n\
 ");
}

int main(int ac, char **av) {
 mjrSession sess;
 char *infile,*outfile;
 int opt_R,nr;

 opt_R=0;
 infile = outfile = NULL;

 while ((nr=getopt(ac, av, "i:o:R"))!=-1) {
   switch(nr) {
    case 'i':
     infile = optarg;
    break;
    case 'R':
     opt_R = 1;
     break;
    case 'o':
     outfile = optarg;
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

 if (!mjrInitSession(&sess)) {
  printf("mjrInitSession faild.\n");
  exit(1);
  }

 sess.obj = elfsh_map_obj(infile);

 if (sess.obj == NULL) {
  printf("elfsh_map_obj faild.\n");
  exit(1);
 }

 mjrSetupProcessor(&sess);
 mjrAnalize(&sess,NULL);

 if (opt_R) {
  mjrSymtabRebuild(&sess);
 }

 if (outfile)
     elfsh_save_obj(sess.obj,outfile);

 return 0;
}
