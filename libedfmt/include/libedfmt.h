/*
** libedfmt.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#ifndef __LIBEDFMT_H__
#define __LIBEDFMT_H__

#include <libelfsh.h>
#include "libedfmt-dwarf2.h"

/* Defines */
#define UNKNOWN_FORMAT 	0
#define DWARF1_FORMAT 	1
#define DWARF2_FORMAT 	2

/* Structures */
typedef struct 	s_edfmtmanage
{
  char 		*sect_name;
  u_int		sect_hash;
  u_int		format;
} 		edfmtmanage_t;

/* edfmt.c */
int		edfmt_format(elfshobj_t *file);
elfshsect_t 	*edfmt_format_find(elfshobj_t *file, u_int *format);
elfshsect_t    	*edfmt_get_sect(elfshobj_t *file, u_int hash, char *hash_name);

/* dwarf2.c */
int		edfmt_dwarf2_parse(elfshobj_t *file);
int		edfmt_dwarf2_block_entrie(elfshobj_t *file);
int		edfmt_dwarf2_block_loop(u_int endpos);

/* dwarf2-utils.c */
char 		*edfmt_dwarf2_ckey(char *buf, u_int size, long key);
char 		*edfmt_dwarf2_cline(char *buf, u_int size, u_int line, char *file);
char 		*edfmt_dwarf2_caddr(char *buf, u_int size, elfsh_Addr addr);
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
