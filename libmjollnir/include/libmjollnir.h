
/*
 * (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
 * BSD License
 * $Id: libmjollnir.h,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $
 *
 */

#ifndef _LIB_MJR_H_
#define _LIB_MJR_H_ 1

#include <sys/types.h>
#include <unistd.h>
#include <libasm.h>
#include <libelfsh.h>

/* For fingerprinting */
#include <md5.h>

#define BSIZE 4096

#define MJR_DB_LINE_MAX_LENGTH	1024
#define MJR_MAX_FNG 1000000

/* Just to prevent allocation of unlimited memmory */
#define	MAX_FUNCTION_LEN 1024*1024

#define	FNG_TYPE_MD5 0
#define FNG_TYPE_SHA1 1
#define	FNG_TYPE_CFLOW 2
#define FNG_TYPE_DFLOW 3

typedef struct _Mjr_fsym Mjr_fsym;
typedef struct _fingerPrint Mjr_fingerPrint;
typedef struct _fingerPrintList Mjr_fprintList;

struct _Mjr_fsym {
 u_int vaddr;
 u_int epoint;
 char  *name;
 char  *md5sum;
#define F_TYPE_CALL 0
#define F_TYPE_SYSCALL 1
 int   type;
};

/**
 * This structure describes an fingerprint
 * in DB
 */

struct _fingerPrint {
  void *data;				/* ptr to fingerprint */
  u_int arch;				/* architecture */
  char *os;					/* os */
  char *rel;				/* os release */
  char *fname;				/* function name */
  u_int addtime;			/* time of addition */
  char *srcFile;			/* source file */
#define F_FLAG_NONE 0
#define F_FLAG_VULN 1
  int flag;					/* flags */
};


struct _fingerPrintList {
 Mjr_fingerPrint *cur;
 Mjr_fingerPrint *next;
 Mjr_fingerPrint *prev;
};

#include <libmjollnir-int.h>
#include <libmjollnir-flow.h>

#endif /* _LIB_MJR_H_ */
