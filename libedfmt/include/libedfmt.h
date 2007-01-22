/*
** libedfmt.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#ifndef __LIBEDFMT_H__
#define __LIBEDFMT_H__

#include <libelfsh.h>

/* Defines */
#define UNKNOWN_FORMAT 	0
#define STABS_FORMAT 	1
#define DWARF2_FORMAT 	2

#define EDFMT_CLINE_SIZE  30
#define EDFMT_CADDR_SIZE  30
#define EDFMT_CKEY_SIZE   30

#include "libedfmt-dwarf2.h"
#include "libedfmt-stabs.h"

/* Structures */
typedef struct 	s_edfmtmanage
{
  char 		*sect_name;
  u_int		sect_hash;
  int		(*func)(elfshobj_t*);
} 		edfmtmanage_t;

/* edfmt.c */
int		edfmt_format(elfshobj_t *file);
elfshsect_t    	*edfmt_get_sect(elfshobj_t *file, u_int hash, char *hash_name, 
				u_int strhash);
/* edfmt-utils.c */
char 		*edfmt_ckey(char *buf, u_int size, long key);
char 		*edfmt_cline(char *buf, u_int size, u_int line, char *file);
char 		*edfmt_caddr(char *buf, u_int size, elfsh_Addr addr);

/* stabs.c */
int		edfmt_stabs_parse(elfshobj_t *file);

/* stabs-utils.c */
char		*edfmt_stabs_readstr(char *buf, u_int size, char **str, char c_delim);
int		edfmt_stabs_readnumber(char **str, char c_delim, long *set_num);
int		edfmt_stabs_typenum(edfmtstabstypenum_t *tnum, char **str);
int		edfmt_stabs_ctypenum(char *buf, u_int size, edfmtstabstypenum_t *tnum);

/* dwarf2.c */
int		edfmt_dwarf2_parse(elfshobj_t *file);
int		edfmt_dwarf2_block_entrie(elfshobj_t *file);
int		edfmt_dwarf2_block_loop(u_int endpos);

/* dwarf2-utils.c */
u_long 		edfmt_read_uleb128(void *data, u_int *bread);
long 		edfmt_read_leb128(void *data, u_int *bread);

/* dwarf2-abbrev.c */
int	      	edfmt_dwarf2_loc(edfmtdw2loc_t *loc, char *buf, u_int size);
int		edfmt_dwarf2_line(u_long offset);
int		edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, u_int line, u_int column, elfsh_Addr addr, u_int fid);
int		edfmt_dwarf2_mac(u_long offset);
int		edfmt_dwarf2_abbrev();
int		edfmt_dwarf2_form(u_int endpos);
edfmtdw2abbent_t *edfmt_dwarf2_lookup_abbrev(u_int num_fetch);

/* dwarf2-cfa */
int		edfmt_dwarf2_cfa();

#endif
