
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: fngdb.c,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $
 */

#include <libmjollnir.h>

/**
 Load MD5 database
*/

int mjr_load_md5_db(char *path,hash_t *db) {
 FILE *f;
 char tmp[MJR_DB_LINE_MAX_LENGTH+4];
 Mjr_fingerPrint *fp;

 f = fopen(path,"r");
 if (f == NULL) {return -1;}

 memset(tmp,0,MJR_DB_LINE_MAX_LENGTH+4);

 hash_init(db,MJR_MAX_FNG);

 while(fgets(tmp,MJR_DB_LINE_MAX_LENGTH,f)) {
  char *brkb,*phrase;
  int pos;

  fp = malloc(sizeof(Mjr_fingerPrint));
  pos = 0;

  for (phrase = strtok_r(tmp, ":", &brkb);
   phrase;
   phrase = strtok_r(NULL, ":", &brkb))
  {
   switch(pos) {
    case 0:
     fp->data = strdup(phrase);
	 break;
	case 1:
	 fp->arch = strtol(phrase,(char **) NULL, 10);
	 break;
	case 2:
	 fp->os = strdup(phrase);
	 break;
	case 3:
	 fp->rel = strdup(phrase);
	 break;
	case 4:
	 fp->fname = strdup(phrase);
	 break;
	case 5:
	 fp->srcFile = strdup(phrase);
	 break;
   }
   pos++;
  }

  hash_add(db,fp->data,fp);

  memset(tmp,0,MJR_DB_LINE_MAX_LENGTH+4);
 }
 
 return 1;
}
