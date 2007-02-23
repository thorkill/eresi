/*
** libedfmt.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#ifndef __LIBEDFMT_H__
#define __LIBEDFMT_H__

#include "libelfsh.h"
#include "libaspect.h"

/* Defines */
#define 	UNKNOWN_FORMAT 			0
#define 	STABS_FORMAT 			1
#define 	DWARF2_FORMAT 			2

#define 	EDFMT_CLINE_SIZE  		30
#define 	EDFMT_CADDR_SIZE  		30
#define 	EDFMT_CKEY_SIZE   		30

/* Debug Flags */
#define 	__DEBUG_STABS__ 		0
#define 	__DEBUG_DWARF2__ 		0

/* Include debug format headers */
#include "libedfmt-api.h"
#include "libedfmt-dwarf2.h"
#include "libedfmt-stabs.h"

/* Structures */
typedef struct 		s_edfmtmanage
{
  char 			*sect_name;		/* Section name */
  u_int			sect_hash;		/* Section hash */
  int			(*func)(elfshobj_t*);	/* Management function */
  int			(*trans)(elfshobj_t*);	/* Transform function */
} 			edfmtmanage_t;

/* edfmt.c */
int		edfmt_format(elfshobj_t *file);
elfshsect_t    	*edfmt_get_sect(elfshobj_t *file, u_int hash, char *hash_name, 
				u_int strhash);
/* edfmt-utils.c */
char 		*edfmt_ckey(char *buf, u_int size, long key);
char 		*edfmt_cline(char *buf, u_int size, u_int line, char *file);
char 		*edfmt_caddr(char *buf, u_int size, elfsh_Addr addr);
elfsh_Addr     	edfmt_lookup_addr(elfshobj_t *file, char *param);

/* api.c */
int		edfmt_uni_print(elfshobj_t *file);
int		edfmt_add_init(elfshobj_t *file);
int		edfmt_add_end();
int		edfmt_update_type(edfmttype_t *type);
int		edfmt_update_var(edfmtvar_t *var);
int		edfmt_change_type_nfile(edfmttype_t *type);
edfmtfile_t   	*edfmt_restore_parent_file();
edfmtfile_t	*edfmt_get_current_file();
edfmtfile_t	*edfmt_get_inc_file(edfmtfile_t *file, char *name);
edfmtfile_t    	*edfmt_add_file(edfmtfile_t *parent_file, char *name, elfsh_Addr start, elfsh_Addr end);
int		edfmt_reset_file();
int		edfmt_reactive_file();
edfmtinfo_t    	*edfmt_get_uniinfo(elfshobj_t *file);
edfmttype_t	*edfmt_add_type_unk(char *name);
edfmttype_t	*edfmt_add_type_basic(char *name, int size);
edfmttype_t	*edfmt_add_type_array(char *name, int size, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_ptr(char *name, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_struct(char *name, int size);
edfmttype_t	*edfmt_add_type_union(char *name, int size);
edfmttype_t	*edfmt_add_type_attr(edfmttype_t *tstruct, char *name, 
				     int start, int size, edfmttype_t *type);
edfmttype_t	*edfmt_add_type_void(char *name);
edfmttype_t	*edfmt_add_type_link(char *name, edfmttype_t *type);

edfmtvar_t	*edfmt_add_var_global(edfmttype_t *type, char *name, 
				      elfsh_Addr addr);

/* stabs.c */
edfmtstabsinfo_t *edfmt_stabs_getinfo(elfshobj_t *file);
int		edfmt_stabs_parse(elfshobj_t *file);

/* stabs-trans.c */
int		edfmt_stabs_transform(elfshobj_t *file);

/* stabs-utils.c */
char		*edfmt_stabs_readstr(char *buf, u_int size, char **str, char c_delim);
int		edfmt_stabs_readnumber(char **str, char c_delim, long *set_num);
int		edfmt_stabs_typenum(edfmtstabstypenum_t *tnum, char **str);
int		edfmt_stabs_ctypenum(char *buf, u_int size, edfmtstabstypenum_t *tnum);

/* dwarf2.c */
edfmtdw2info_t 	*edfmt_dwarf2_getinfo(elfshobj_t *file);
int		edfmt_dwarf2_parse(elfshobj_t *file);
int		edfmt_dwarf2_block_entrie(elfshobj_t *file);
int		edfmt_dwarf2_block_loop(u_int endpos);
int		edfmt_dwarf2_build_tree(edfmtdw2abbent_t *parent,
					edfmtdw2abbent_t *fent,
					edfmtdw2abbent_t *ref);

/* dwarf2-trans.c */
int		edfmt_dwarf2_transform(elfshobj_t *file);
edfmtdw2abbattr_t *edfmt_dwarf2_getattr(edfmtdw2abbent_t *abbrev, u_int attr);

/* dwarf2-utils.c */
u_long 		edfmt_read_uleb128(void *data, u_int *bread);
long 		edfmt_read_leb128(void *data, u_int *bread);

/* dwarf2-abbrev.c */
int	      	edfmt_dwarf2_loc(edfmtdw2loc_t *loc, char *buf, u_int size);
int		edfmt_dwarf2_line(u_long offset);
int		edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, u_int line, u_int column, elfsh_Addr addr, u_int fid);
int		edfmt_dwarf2_mac(u_long offset);
edfmtdw2abbent_t *edfmt_dwarf2_abbrev_read(u_long pos, u_long ipos);
int		edfmt_dwarf2_abbrev_enum();
int		edfmt_dwarf2_abbrev_enum_clean();
int		edfmt_dwarf2_form(u_int endpos);
u_long		 *edfmt_dwarf2_lookup_abbrev(u_int num_fetch);

/* dwarf2-cfa */
int		edfmt_dwarf2_cfa();

#endif
