/*
** libedfmt.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
**
** $Id: libedfmt.h,v 1.16 2007-03-07 16:45:35 thor Exp $
**
*/

#ifndef __LIBEDFMT_H__
#define __LIBEDFMT_H__

#include "libelfsh.h"

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

/* Allocate pool steps */
#define		DWARF2_ALLOC_STEP		4096
#define		STABS_ALLOC_STEP		4096
#define		API_ALLOC_STEP			4096

/* Include debug format headers */
#include "libedfmt-api.h"
#include "libedfmt-dwarf2.h"
#include "libedfmt-stabs.h"

/* Structures */
typedef struct 		s_edfmtmanage
{
  char 			*sect_name;		/* Section name */
  unsigned int			sect_hash;		/* Section hash */
  int			(*func)(elfshobj_t*);	/* Management function */
  int			(*trans)(elfshobj_t*);	/* Transform function */
  int			(*clean)(elfshobj_t*);	/* Clean function */
} 			edfmtmanage_t;

/* api.c */
int		edfmt_add_init(elfshobj_t *file);
int		edfmt_add_end();
int		edfmt_update_type(edfmttype_t *type);
int		edfmt_update_var(edfmtvar_t *var);
int		edfmt_update_func(edfmtfunc_t *func);
int		edfmt_change_type_nfile(edfmttype_t *type);
edfmtfile_t   	*edfmt_restore_parent_file();
edfmtfile_t	*edfmt_get_current_file();
edfmtfile_t	*edfmt_get_inc_file(edfmtfile_t *file, char *name);
edfmtfile_t    	*edfmt_add_file(edfmtfile_t *parent_file, char *name, eresi_Addr start, eresi_Addr end);
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
				      eresi_Addr addr);
edfmtfunc_t	*edfmt_add_func(char *name, edfmttype_t *ret, 
				eresi_Addr start, eresi_Addr end);
edfmtfuncarg_t	*edfmt_add_arg(edfmtfunc_t *func, char *name,
			       unsigned int reg, int pos, edfmttype_t *type);
edfmttype_t	*edfmt_check_type(char *name);
edfmtvar_t	*edfmt_check_var(char *name);
edfmtfunc_t	*edfmt_check_func(char *name);

/* edfmt.c */
int		edfmt_format(elfshobj_t *file);
elfshsect_t    	*edfmt_get_sect(elfshobj_t *file, unsigned int hash, char *hash_name, 
				unsigned int strhash);
/* edfmt-utils.c */
char 		*edfmt_ckey(char *buf, unsigned int size, long key);
char 		*edfmt_cline(char *buf, unsigned int size, unsigned int line, char *file);
char 		*edfmt_caddr(char *buf, unsigned int size, eresi_Addr addr);
eresi_Addr     	edfmt_lookup_addr(elfshobj_t *file, char *param);
void 	       	*edfmt_alloc_pool(char **pool, int *apos, int *asize, 
				  int astep, int nsize);
int 		edfmt_clean_pool(char **pool);

/* stabs.c */
edfmtstabsinfo_t *edfmt_stabs_getinfo(elfshobj_t *file);
int		edfmt_stabs_parse(elfshobj_t *file);
int		edfmt_stabs_clean(elfshobj_t *file);

/* stabs-trans.c */
int		edfmt_stabs_transform(elfshobj_t *file);

/* stabs-utils.c */
char		*edfmt_stabs_readstr(char *buf, unsigned int size, char **str, char c_delim);
int		edfmt_stabs_readnumber(char **str, char c_delim, long *set_num);
int		edfmt_stabs_typenum(edfmtstabstypenum_t *tnum, char **str);
int		edfmt_stabs_ctypenum(char *buf, unsigned int size, edfmtstabstypenum_t *tnum);

/* dwarf2.c */
edfmtdw2info_t 	*edfmt_dwarf2_getinfo(elfshobj_t *file);
int		edfmt_dwarf2_parse(elfshobj_t *file);
int 		edfmt_dwarf2_clean(elfshobj_t *file);
int		edfmt_dwarf2_block_entrie(elfshobj_t *file);
int		edfmt_dwarf2_block_loop(unsigned int endpos);
int		edfmt_dwarf2_build_tree(edfmtdw2abbent_t *parent,
					edfmtdw2abbent_t *fent,
					edfmtdw2abbent_t *ref);
int 		edfmt_dwarf2_getent(edfmtdw2cu_t *cu, 
				    edfmtdw2abbent_t *abbent, 
				    unsigned int pos);

/* dwarf2-trans.c */
int		edfmt_dwarf2_transform_abbrev(unsigned int pos);
edfmttype_t    	*edfmt_dwarf2_transform_abbrev_parse(edfmtdw2abbent_t *abbrev);
int		edfmt_dwarf2_transform(elfshobj_t *file);
edfmtdw2abbattr_t *edfmt_dwarf2_getattr(edfmtdw2abbent_t *abbrev, unsigned int attr);

/* dwarf2-utils.c */
unsigned long 		edfmt_read_uleb128(void *data, unsigned int *bread);
long 		edfmt_read_leb128(void *data, unsigned int *bread);

/* dwarf2-abbrev.c */
int	      	edfmt_dwarf2_loc(edfmtdw2loc_t *loc, unsigned char *buf, unsigned int size);
int		edfmt_dwarf2_line(unsigned long offset);
int		edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, unsigned int line, unsigned int column, eresi_Addr addr, unsigned int fid);
int		edfmt_dwarf2_mac(unsigned long offset);
int		edfmt_dwarf2_abbrev_read(edfmtdw2abbent_t *abbent, unsigned long pos, unsigned long ipos);
int		edfmt_dwarf2_abbrev_enum(hash_t *abbrev_table);
int		edfmt_dwarf2_abbrev_enum_clean(hash_t *abbrev_table);
int		edfmt_dwarf2_form(edfmtdw2abbent_t *abbent, unsigned int pos);
int 	   	edfmt_dwarf2_form_value(edfmtdw2abbattr_t *attr);

/* dwarf2-cfa */
int		edfmt_dwarf2_cfa();

#endif
