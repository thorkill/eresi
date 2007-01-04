/*
** libedfmt.h for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 25 2006 15:41:35 mxatone
**
*/

#ifndef __LIBEDFMT_H__
#define __LIBEDFMT_H__

#include <libelfsh.h>
#include "dwarf2.h"

/* Defines */
#define UNKNOWN_FORMAT 	0
#define DWARF1_FORMAT 	1
#define DWARF2_FORMAT 	2

#define DWARF1_SECTION 	".debug"
#define DWARF2_SECTION 	".debug_info"

/* DWARF2 sections */
#define DWARF2_SECTION_ARANGES 	".debug_aranges"
#define DWARF2_SECTION_PUBNAMES	".debug_pubnames"
#define DWARF2_SECTION_ABBREV	".debug_abbrev"
#define DWARF2_SECTION_LINE	".debug_line"
#define DWARF2_SECTION_FRAME	".debug_frame"
#define DWARF2_SECTION_STR	".debug_str"
#define DWARF2_SECTION_MACINFO	".debug_macinfo"

/* Structures */
typedef struct 	s_edfmtmanage
{
  char 		*sect_name;
  unsigned int	format;
} 		edfmtmanage_t;


/* edfmt.c */
int		edfmt_format(elfshobj_t *file);
elfshsect_t 	*edfmt_format_find(elfshobj_t *file, u_int *format);

/* dwarf2.c */
char 		*edfmt_dwarf2_ckey(char *buf, u_int size, long int key);
int		edfmt_dwarf2_parse(elfshobj_t *file, elfshsect_t *sect);
int		edfmt_dwarf2_block_entrie();
int		edfmt_dwarf2_block_loop(u_int endpos);
int		edfmt_dwarf2_abbrev();
int		edfmt_dwarf2_form(u_int endpos);
int		edfmt_dwarf2_form_value(edfmtdw2abbattr_t *attr);
edfmtdw2abbent_t *edfmt_dwarf2_lookup_abbrev(u_int num_fetch);
#endif
