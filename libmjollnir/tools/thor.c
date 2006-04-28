/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: thor.c,v 1.2 2006-04-28 06:56:33 thor Exp $
 *
 * Since this library is build on top of libasm and elfsh
 * I decided to use mydisasm.c from libasm as a prototype
 *
 * Credits for asm: <sk at devhell dot org>
 *             elfsh: <elfsh at devhell dot org>
 *             checksums: lcamtuf
 */

/*
 WIP - Status!
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libelfsh.h>
#include <libasm.h>
#include <libmjollnir.h>

int	usage(char *p) {
  printf("Usage: %s -i <input binary>\n", p);
  printf("\t[-o <output file>]\n");
  printf("\t[-c]   - print fingerprints in db format\n");
  printf("\t[-O OS]\n");
  printf("\t[-A arch]\n");
  printf("\t[-F <srcFile>]\n");
  return (-1);
}

int	main(int ac, char **av) {
  int cnt2,x,nr;
  char **ret;
  char *infile,*outfile,*os,*srcFile,*arch,*osRel,*shtName;
  int opt_cdb;
  u_int num_sht,idx_sht;
  Mjr_fingerPrint *fng;
  Mjr_fsym *fs;
  hash_t *hs;
  elfsh_Shdr *shtlist;
  elfsh_Shdr *shdr;
  elfsh_Sym  *sym;
  Mjr_CTX ctx;

 hs=NULL;

 infile = NULL; 		/* input file */
 outfile = NULL;		/* new recovered file */
 arch = NULL;			/* arch */
 os = NULL;				/* os i.e. FreeBSD */
 osRel = NULL;			/* os release 5.4-STABLE */
 srcFile = NULL;		/* source file i.e. printf.o */
 opt_cdb = 0; 			/* create fingerprints db */

 while ((nr=getopt(ac, av, "ci:O:A:F:R:o:"))!=-1) {
   switch(nr) {
      case 'i':
	   infile = optarg;
	   break;
	  case 'c':
	   opt_cdb = 1;
	   break;
	  case 'A':
	   arch = optarg;
	   break;
	  case 'O':
	   os = optarg;
	   break;
	  case 'R':
	   osRel = optarg;
	   break;
	  case 'F':
	   srcFile = optarg;
	   break;
	  case 'o':
	   outfile = optarg;
	   break;
	  default:
	   return(usage(av[0]));
   }
 }

 if (infile == NULL) {
	  return(usage(av[0]));
 }

 if (srcFile == NULL) {
  srcFile = infile;
 }


  mjr_init(&ctx);
  asm_init_i386(&ctx.proc);

  /* load obj */
  ctx.obj = elfsh_map_obj(infile);

  if (!ctx.obj)
    {
      printf("faild opening %s\n", infile);
      return -1;
    }

  if (opt_cdb == 0) {

   /* load the MD5 database */
   if (mjr_load_md5_db("./share/mjollnirMD5.db",(hash_t *)&ctx.md5db) == -1) {
    printf("can't load DB\n");return -1;
   }


  if (!(shtlist = elfsh_get_sht(ctx.obj, &num_sht))) {
    printf("faild to get sht!\n");
	return -1;
  }

   printf("[i] Found %d sections.\n",num_sht);


  for (idx_sht = 0; idx_sht < num_sht; idx_sht++) {
   shdr = (shtlist + idx_sht);
   sym = elfsh_get_sym_from_shtentry(ctx.obj, shdr);
   shtName = elfsh_get_symbol_name(ctx.obj, sym);

   if (elfsh_get_section_execflag(shdr)) {
    printf("[i] Section name=(%14s) index=(%02i)\n", shtName, idx_sht);

    /* scan the raw data for functions */
    if (mjr_find_calls_raw(&ctx,(hash_t *)&hs,shtName,"sub_") == -1) {
      printf("raw read failed\n");return -1;
    }

  ret = hash_get_keys((hash_t *)&hs,&cnt2);

   for(x=0;x<cnt2;x++) {
    fs = hash_get((hash_t *)&hs,ret[x]);
    if (fs->type == F_TYPE_CALL) {

     fng = hash_get((hash_t *)&ctx.md5db,fs->md5sum);

     /* We found it */
     if (fng != NULL) {
       printf("[!] VADDR:0x%08x (0x%08x) MD5:%s TMP:%s SYMBOL:%s OS:%s REL:%s\n",
 	   fs->epoint,
 	   fs->vaddr,
 	   (char *)fng->data,
 	   ret[x],
	   fng->fname,
	   fng->os,
	   fng->rel);

     if (outfile != NULL)
	  mjr_add_symbol(ctx.obj,shtName,fs->vaddr,fng->fname);

    /* it sucks - no fingerprint matched*/
    } else {
     printf("[-] VADDR:0x%08x (0x%08x) MD5:%s TMP:%s SYMBOL:%s OS:%s REL:%s\n",
	  fs->epoint,
	  fs->vaddr,
	  fs->md5sum,
	  ret[x],
	  "-",
	  "-",
	  "-"
	  );

     if (outfile != NULL)
      mjr_add_symbol(ctx.obj,shtName,fs->vaddr,ret[x]);
    }

	/* do the syscall stuff */
	} else if (fs->type == F_TYPE_SYSCALL) {
	 printf("[S] VADDR:0x%08x SYSCALL NR: %d SYMBOL:%s\n",
	 fs->vaddr,
	 fs->epoint,
	 ret[x]
	 );

     if (outfile != NULL)
      mjr_add_symbol(ctx.obj,shtName,fs->vaddr,ret[x]);

     }
	}
   }
  }

  } else if (opt_cdb == 1) {

   /* fill the hash using symbol table lookup */
   mjr_find_calls_symtab(&ctx,(hash_t *)&hs);

   ret = hash_get_keys((hash_t *)&hs,&cnt2);
   for(x=0;x<cnt2;x++) {

    fs = hash_get((hash_t *)&hs,ret[x]);

	printf("%s:%s:%s:%s:%s:%s:\n",
	 fs->md5sum,
	 (arch != NULL ? arch : "U"),
	 (os != NULL ? os : "U"),
	 (osRel != NULL ? osRel : "U"),
	 fs->name,
	 srcFile
	 );
   }

  }

  /* save new object */
  if (outfile != NULL)
   elfsh_save_obj(ctx.obj,outfile);

  elfsh_unload_obj(ctx.obj);

  printf("Stats: CALLs seen: %d Recognized: %d\n",
   ctx.stats_calls_seen,
   ctx.stats_calls_found);
  return (0);
}
