/*
** dwarf2.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

/* This file implements DWARF2 transformation */

#define DWARF2_HNAME_TRANS_TREF "dwarf2_trans_typeref"

/* Temporary buffer outside function to optimize function recursion */
char buf[BUFSIZ];

/* Transform hash table */
hash_t types_ref;

/* Search a type from its ckey string */
/*
static edfmttype_t	*edfmt_dwarf2_searchtype(char *ckey)
{
  edfmttype_t 		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  type = (edfmttype_t *) hash_get(&types_ref, ckey);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

// Search a type from its ckey long
static edfmttype_t 	*edfmt_dwarf2_searchtypei(long ckey)
{
  edfmttype_t 		*type;
  char			str_ckey[EDFMT_CKEY_SIZE];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  // Create the string
  edfmt_ckey(str_ckey, EDFMT_CKEY_SIZE, ckey);

  // Retrieve the type 
  type = edfmt_dwarf2_searchtype(str_ckey);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}
*/

static edfmttype_t	*edfmt_dwarf2_searchtype_str(char *str)
{
  edfmttype_t 		*type = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  type = (edfmttype_t *) hash_get(&types_ref, str);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}
static edfmttype_t	*edfmt_dwarf2_searchtype(edfmtdw2abbent_t *abbrev)
{
  edfmttype_t 		*type = NULL;
  edfmtdw2abbattr_t	*attr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  attr = edfmt_dwarf2_getattr(abbrev, DW_AT_name);

  if (attr)
    type = (edfmttype_t *) hash_get(&types_ref, attr->u.str);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

/* Get a type starting by try to resolve it fast */
static edfmttype_t	*edfmt_dwarf2_trans_gettype(u_int pos)
{
  edfmtdw2abbent_t   	ref;
  edfmttype_t 		*type;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (pos <= 0 || current_cu == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  if (edfmt_dwarf2_getent(current_cu, &ref, current_cu->start_pos + pos) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "dwarf2 getenv failed", NULL);

  type = edfmt_dwarf2_searchtype(&ref);

  if (type == NULL)
    type = edfmt_dwarf2_transform_abbrev_parse(&ref);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

edfmtdw2abbattr_t 	*edfmt_dwarf2_getattr(edfmtdw2abbent_t *abbent, u_int attr)
{
  edfmtdw2info_t	*pinfo;
  u_int			i;
  u_char		*bufptr;
  u_short		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (abbent != NULL)
    {
      for (i = 0; abbent->attr[i].attr; i++)
	{
	  if (abbent->attr[i].attr == attr)
	    {
	      /* Save current pointers */
	      pinfo = dwarf2_info;
	      dwarf2_info = (edfmtdw2info_t *) current_cu->fileobj->debug_format.dwarf2;

	      edfmt_dwarf2_form_value(abbent->attr + i);
	      
	      /* Modifications */
	      switch(abbent->attr[i].attr)
		{
		case DW_AT_frame_base:
		case DW_AT_data_member_location:
		case DW_AT_location:
		  switch(abbent->attr[i].form)
		    {
		    case DW_FORM_block1:
		      edfmt_dwarf2_loc(&(abbent->attr[i].loc), (u_char *) abbent->attr[i].u.vbuf, 
				       abbent->attr[i].asize);
		      break;
		      /* We have to use .debug_loc */
		    case DW_FORM_data8:
		    case DW_FORM_data4:
		      /* We need loc section */
		      if (dwarf2_data(loc) == NULL)
			break;

		      /* Set position */
		      dwarf2_pos(loc) = abbent->attr[i].u.udata;
		      bufptr = dwarf2_a_pos(loc);

		      /* Read length, we don't care about start & end */
		      len = *(u_short *) (bufptr + (current_cu->addr_size * 2));
	      
		      /*printf("%x %x len = %d / %d\n", 
		       *(elfsh_Addr *) bufptr,
		       *(elfsh_Addr *) bufptr + current_cu->addr_size,
		       len, dwarf2_pos(loc));*/

		      dwarf2_pos(loc) += (current_cu->addr_size * 2) + sizeof(u_short);

		      /*if (len > 20)
			asm("int3");*/

		      if (dwarf2_pos(loc) + len > dwarf2_size(loc))
			{
			  printf("TOOBIG !!\n");
			  break;
			}
	      
		      edfmt_dwarf2_loc(&(abbent->attr[i].loc), 
				       dwarf2_a_pos(loc), len);		

		      break;
		    }
		  break;
		  /* TODO: reuse when it will be usefull
		     case DW_AT_stmt_list:
		     edfmt_dwarf2_line(ent->attr[i].u.udata);
		     break;
		     case DW_AT_macro_info:
		     edfmt_dwarf2_mac(ent->attr[i].u.udata);
		     break;
		  */
		}

	      /* Restore ex pointers */
	      dwarf2_info = pinfo;

	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, abbent->attr + i);
	    }
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

elfsh_Addr		edfmt_dwarf2_getaddr(char *vbuf)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!vbuf)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", (elfsh_Addr) 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, *(elfsh_Addr *) vbuf);
}

edfmttype_t		*edfmt_dwarf2_transform_abbrev_parse(edfmtdw2abbent_t *abbrev)
{
  char			*str = NULL, *comp_dir, *vbuf, *vbufs, *pstr;
  long			size = 0;
  edfmttype_t		*type = NULL, *etype;
  u_long		iref, itref;
  edfmtdw2abbent_t	ref, tref;
  edfmtdw2abbattr_t	*attr;
  elfsh_Addr		low = 0, high = 0;
  int			fileid, inc = 0, addtype = 1, base = 0, op = 0;
  edfmtfunc_t		*func;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!abbrev)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", NULL);

  /* Retrieve abbrev name */
  DWARF2_TRANS_GETATTR(str, abbrev, DW_AT_name, u.str, NULL);

  //type = edfmt_dwarf2_searchtype(abbrev->ckey);
  if (str)
    {
      type = edfmt_dwarf2_searchtype_str(str);
      
      /* Already parsed ? */
      if (type)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
    }

  /* Change current file */
  if (abbrev->tag != DW_TAG_compile_unit)
    {
      DWARF2_TRANS_GETATTR(fileid, abbrev, DW_AT_decl_file, u.udata, -1);

      inc = 1;
      if (fileid > 0 && fileid < current_cu->files_number)
	{
	  fileid--;
	  pstr = current_cu->dirs[current_cu->files_dindex[fileid] - 1];
	  if (pstr != NULL 
	      && current_cu->files_dindex[fileid] < current_cu->files_number
	      && current_cu->files_dindex[fileid] > 0)
	    {
	      size = strlen(pstr);
	      snprintf(buf, BUFSIZ - 1, "%s%s%s",
		       pstr, 
		       pstr[size - 1] != '/' ? "/" : "",
		       current_cu->files_name[fileid]);
	    }
	  else
	    {
	      snprintf(buf, BUFSIZ - 1, "%s",
		       current_cu->files_name[fileid]);
	    }
	  
	  edfmt_add_file(edfmt_get_current_file(), buf, 0, 0);
	  
	  pstr = NULL;
	  size = 0;
	}
      else if (fileid >= 0)
	{
	  inc++;
	  edfmt_reset_file();
	}
    }

  switch(abbrev->tag)
    {
    case DW_TAG_compile_unit:
      if (!str)
	break;

      if (str[0] != '/')
	{
	  DWARF2_TRANS_GETATTR(comp_dir, abbrev, DW_AT_comp_dir, u.str, NULL);

	  if (comp_dir)
	    {
	      snprintf(buf, BUFSIZ - 1, "%s%s%s",
		       comp_dir, 
		       comp_dir[strlen(comp_dir) - 1] != '/' ? "" : "/",
		       str);
	      str = buf;
	    }
	}

      DWARF2_TRANS_GETATTR(vbuf, abbrev, DW_AT_low_pc, u.vbuf, NULL);
      DWARF2_TRANS_GETATTR(vbufs, abbrev, DW_AT_high_pc, u.vbuf, NULL);

      /* Add a new file */
      edfmt_add_file(NULL, str, 
		     edfmt_dwarf2_getaddr(vbuf),
		     edfmt_dwarf2_getaddr(vbufs));
      break;
    case DW_TAG_variable:
      attr = edfmt_dwarf2_getattr(abbrev, DW_AT_location);

      if (!attr || attr->loc.op != DW_OP_addr)
	break;

      DWARF2_TRANS_GETATTR(iref, abbrev, DW_AT_type, u.udata, 0);

      type = edfmt_dwarf2_trans_gettype(iref);

      if (!type)
	break;

      edfmt_add_var_global(type, str, attr->loc.value);
      break;
    case DW_TAG_base_type:
      DWARF2_TRANS_GETATTR(size, abbrev, DW_AT_byte_size, u.udata, -1);
      type = edfmt_add_type_basic(str, (int) size);
      break;
    case DW_TAG_typedef:
      DWARF2_TRANS_GETATTR(iref, abbrev, DW_AT_type, u.udata, 0);

      type = edfmt_dwarf2_trans_gettype(iref);

      if (!type)
	break;

      type = edfmt_add_type_link(str, type);
      break;
    case DW_TAG_pointer_type:
    case DW_TAG_reference_type:
      DWARF2_TRANS_GETATTR(iref, abbrev, DW_AT_type, u.udata, 0);

      type = edfmt_dwarf2_trans_gettype(iref);

      if (type)
	{
	  snprintf(buf, BUFSIZ - 1, "*%s", type->name);
	}
      else /* We've got a void* */
	{
	  type = edfmt_add_type_void("void");
	  snprintf(buf, BUFSIZ - 1, "void*");
	}

      type = edfmt_add_type_ptr(buf, type);
      break;
    case DW_TAG_array_type:
      if (abbrev->child > 0 && edfmt_dwarf2_getent(current_cu, &tref, abbrev->child) == 0
	  && tref.tag == DW_TAG_subrange_type)
	{
	  DWARF2_TRANS_GETATTR(iref, abbrev, DW_AT_type, u.udata, 0);
	  
	  type = edfmt_dwarf2_trans_gettype(iref);

	  if (!type)
	    break;

	  DWARF2_TRANS_GETATTR(size, &tref, DW_AT_upper_bound, u.udata, -1);

	  snprintf(buf, BUFSIZ - 1, "%s[%d]", type->name, (int) size+1);
	  type = edfmt_add_type_array(buf, (int) size, type);
	}
      break;
    case DW_TAG_structure_type:
    case DW_TAG_union_type:
      if (abbrev->child)
	{
	  DWARF2_TRANS_GETATTR(size, abbrev, DW_AT_byte_size, u.udata, -1);

	  if (!str)
	    {
	      snprintf(buf, BUFSIZ - 1, "s_(%u)", abbrev->key);
	      str = buf;
	    }
	  
	  //printf("Struct: %s ================ \n", str);

	  if (abbrev->tag == DW_TAG_union_type)
	    type = edfmt_add_type_union(str, (int) size);
	  else
	    type = edfmt_add_type_struct(str, (int) size);
	  
	  /* We add our structure right before members to avoid pointers infinite loop */
	  if (type && str)
	    HASH_ADDX(&types_ref, str, (void *) type);

	  if (edfmt_dwarf2_getent(current_cu, &ref, abbrev->child) < 0)
	    break;

	  for (iref = abbrev->child; iref > 0; iref = ref.sib)
	    {
	      if (edfmt_dwarf2_getent(current_cu, &ref, iref) < 0)
		break;

	      /* Do we have a member */
	      if (ref.tag == DW_TAG_member)
		{
		  DWARF2_TRANS_GETATTR(str, &ref, DW_AT_name, u.str, NULL);
		  DWARF2_TRANS_GETATTR(itref, &ref, DW_AT_type, u.udata, 0);

		  //printf("attr: %s\n", str);

		  if (itref == 0)
		    continue;

		  etype = edfmt_dwarf2_trans_gettype(itref);

		  if (!etype)
		    continue;

		  if (DW_TAG_union_type != abbrev->tag)
		    {
		      attr = edfmt_dwarf2_getattr(&ref, DW_AT_data_member_location);
		      
		      if (!str || !attr)
			continue;

		      base = (int) attr->loc.value;
		    }

		  edfmt_add_type_attr(type, str, base, 0, etype);
		}
	      else
		{
		  edfmt_dwarf2_transform_abbrev_parse(&ref);
		}
	    }

	  /* We won't have to re-parse structure members */
	  abbrev->child = 0;
	  
	  /* We already add our type and we want to return the right information */
	  addtype = 0;
	}
      break;
    case DW_TAG_subprogram:
      DWARF2_TRANS_GETATTR(vbuf, abbrev, DW_AT_low_pc, u.vbuf, 0);
      if (vbuf)
	low = *(elfsh_Addr *) vbuf;
      DWARF2_TRANS_GETATTR(vbuf, abbrev, DW_AT_high_pc, u.vbuf, 0);
      if (vbuf)
	high = *(elfsh_Addr *) vbuf;

      DWARF2_TRANS_GETATTR(iref, abbrev, DW_AT_type, u.udata, 0);
      
      type = edfmt_dwarf2_trans_gettype(iref);
      
      if (!type)
	break;      

      func = edfmt_add_func(str, type, low, high);

      if (func)
	{
	  for (iref = abbrev->child; iref > 0; iref = ref.sib)
	    {
	      if (edfmt_dwarf2_getent(current_cu, &ref, iref) < 0)
		break;

	      /* Do we have a parameter */
	      if (ref.tag == DW_TAG_formal_parameter)
		{
		  DWARF2_TRANS_GETATTR(str, &ref, DW_AT_name, u.str, NULL);
		  DWARF2_TRANS_GETATTR(itref, &ref, DW_AT_type, u.udata, 0);
		  
		  type = edfmt_dwarf2_trans_gettype(itref);
		  attr = edfmt_dwarf2_getattr(&ref, DW_AT_location);
		  
		  if (!type || !attr)
		    continue;      

		  /* Support based reg */
		  if (attr->loc.op >= DW_OP_breg0 && attr->loc.op <= DW_OP_breg31)
		    op = attr->loc.op - DW_OP_breg0;

		  edfmt_add_arg(func, str, op, (int) attr->loc.value, type);
		}
	      else
		{
		  edfmt_dwarf2_transform_abbrev_parse(&ref);
		}
	    }

	  /* We won't have to re-parse function arguments */
	  abbrev->child = 0;
	}
      break;
    }

  /* Restore a parent for a specific include */
  if (inc == 1)
    edfmt_restore_parent_file();
  else if (inc == 2)
    edfmt_reactive_file();

  if (type && addtype && str)
    HASH_ADDX(&types_ref, str, (void *) type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}

/* Use in global scope to improve speed */
#define DW2_MAX_LEVEL 20
edfmtdw2abbent_t     	abbrev[DW2_MAX_LEVEL];
int 			abbrev_level = 0;

int	     		edfmt_dwarf2_transform_abbrev(u_int pos)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (pos <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (abbrev_level >= DW2_MAX_LEVEL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  while (pos > 0 && pos < current_cu->end_pos)
    {
      if (edfmt_dwarf2_getent(current_cu, &(abbrev[abbrev_level]), pos) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Can't retrieve abbrev entitie", -1);

      edfmt_dwarf2_transform_abbrev_parse(&(abbrev[abbrev_level]));

      /* Parse childs too */
      if (abbrev[abbrev_level].child > 0)
	{
	  abbrev_level++;
	  edfmt_dwarf2_transform_abbrev(abbrev[abbrev_level - 1].child);
	  abbrev_level--;
	}

      pos = abbrev[abbrev_level].sib;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Transform dwarf2 informations */
int			edfmt_dwarf2_transform(elfshobj_t *file)
{
  edfmtdw2info_t 	*tinfo;
  edfmtdw2cu_t 		*tcu;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1); 

  tinfo = (edfmtdw2info_t *) file->debug_format.dwarf2;

  if (tinfo == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No debug information for dwarf2", -1); 

  tcu = (edfmtdw2cu_t *) tinfo->cu_list;

  if (tcu == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No compile unit information for dwarf2", -1);   

  if (types_ref.ent == NULL)
    hash_init(&types_ref, DWARF2_HNAME_TRANS_TREF, 200, ASPECT_TYPE_UNKNOW);

  edfmt_add_init(file);

  while (tcu != NULL)
    {
      current_cu = tcu;

      abbrev_level = 0;
      edfmt_dwarf2_transform_abbrev(tcu->info_pos);

      tcu = tcu->next;
    }

  hash_empty(DWARF2_HNAME_TRANS_TREF);

  current_cu = NULL;

  edfmt_add_end();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
