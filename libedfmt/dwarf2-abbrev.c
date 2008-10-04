/*
** @file dwarf2-abbrev.c
** @ingroup libedfmt
** Started Dec 26 2006 10:49:45 mxatone
**
**
** $Id: dwarf2-abbrev.c,v 1.14 2007-05-20 19:13:57 mxatone Exp $
**
*/

#include "libedfmt.h"

char *alloc_pool = NULL;
int alloc_pos = 0;
int alloc_size = 0;

#define DW2_GETPTR(_var, _size, _ret)	 	      	\
do {						     	\
  _var = edfmt_alloc_pool(&alloc_pool, 			\
		       &alloc_pos, 			\
		       &alloc_size, 			\
		       DWARF2_ALLOC_STEP, _size); 	\
if (_var == NULL)					\
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 	\
		 "Pool allocation failed", _ret);     	\
} while (0)

/**
 * Lookup on abbrev_table and retrieve en abbrev entity (structure)
 * @param num_fetch used to found the abbrev entity on the abbrev hash table
 * @return abbrev entity structure
 */
static edfmtdw2abbent_t	*edfmt_dwarf2_lookup_abbrev(unsigned int num_fetch)
{
  char			ckey[EDFMT_CKEY_SIZE];
  edfmtdw2abbent_t 	*res;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  edfmt_ckey(ckey, EDFMT_CKEY_SIZE, num_fetch);
  res = (edfmtdw2abbent_t *) hash_get(&(current_cu->abbrev_table), ckey);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, res);
}

/**
 * Read an abbrev entry @ this position
 * @param ent abbrev entity to fill
 * @param fnum abbrev entity fetch number
 * @param ipos abbrev entity key to set
 */
int			edfmt_dwarf2_abbrev_read(edfmtdw2abbent_t *ent, unsigned long fnum, unsigned long ipos)
{
  edfmtdw2abbent_t	*fetch;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!ent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  /* Do we have this section ? */
  if (dwarf2_data(abbrev) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 ".debug_abbrev section not available", -1);

  fetch = edfmt_dwarf2_lookup_abbrev(fnum);

  if (!fetch)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No entry found", -1);

  memcpy(ent, fetch, sizeof(edfmtdw2abbent_t));

  /* Fill main data */
  ent->key = ipos;
  //edfmt_ckey(ent->ckey, EDFMT_CKEY_SIZE, ipos);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Clean allocated buffer for enumeration 
 * @param abbrev_table store abbrev entities (structure)
 */
int			edfmt_dwarf2_abbrev_enum_clean(hash_t *abbrev_table)
{
  char			**keys;
  unsigned int			index;
  int			keynbr;
  unsigned long		*value;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  if (!abbrev_table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid arguments",  -1);

  /* Clean every elements on the hash table */
  keys = hash_get_keys(abbrev_table, &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  value = (unsigned long *) hash_get(abbrev_table, keys[index]);

	  if (value)
	    XFREE(__FILE__, __FUNCTION__, __LINE__,value);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Parse the .debug_abbrev format and store the result on the abbrev table
 * @param abbrev_table store abbrev entities (structure)
 */
int			edfmt_dwarf2_abbrev_enum(hash_t *abbrev_table)
{
  unsigned int			num;
  unsigned int			base;
  edfmtdw2abbent_t	*ent;
  char			ckey[EDFMT_CKEY_SIZE];
  unsigned int			allocattr = 0;
  unsigned int			attri = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!abbrev_table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid arguments",  -1);

  /* Do we have this section ? */
  if (dwarf2_data(abbrev) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_abbrev section not available",  -1);

  base = dwarf2_pos(abbrev);

  do {
    /* Fetch the key number */
    dwarf2_iuleb128(num, abbrev);

    /* A level end */
    if (num == 0)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

    DW2_GETPTR(ent, sizeof(edfmtdw2abbent_t), -1);

    dwarf2_iuleb128(ent->tag, abbrev);
    dwarf2_iread_1(ent->children, abbrev);
    
    attri = allocattr = 0;
    
    /* Parse attributes */
    do {
      /* Fill attribute structure */
      dwarf2_iuleb128(ent->attr[attri].attr, abbrev);
      dwarf2_iuleb128(ent->attr[attri].form, abbrev);
    } while (ent->attr[attri++].attr && attri < DW2_MAX_ATTR);

    ent->attrsize = attri > 1 ? attri - 1 : attri;
      
    /* Add into abbrev table */
    edfmt_ckey(ckey, EDFMT_CKEY_SIZE, num);
    hash_add(abbrev_table, strdup(ckey), (void *) ent);
  } while (dwarf2_pos(abbrev) < dwarf2_size(abbrev));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Increment the info position without reading every elements
 * then we avoid losing data access time on elements that we don't need.
 * @param attr abbrev attribute to iterate
 */
static int    		edfmt_dwarf2_form_it(edfmtdw2abbattr_t *attr)
{
  long			data;
  unsigned long		size;
  unsigned int			form;

  NOPROFILER_IN();

  /* We keep current position then we can read value for a specific element later */
  attr->infopos = dwarf2_pos(info);
  form = attr->form;

fvstart:

  /**
   * I will not explain every element because it will be too long
   * you can read dwarf2 documentation for more information.
   */
  switch (form)
    {
    case DW_FORM_ref_addr:
      dwarf2_inc_pos(info, current_cu->addr_size);
      break;
    case DW_FORM_addr:
      dwarf2_inc_pos(info, current_cu->addr_size);
      break;
    case DW_FORM_block1:
      dwarf2_iread_1(size, info);
      if (size > 0)
	dwarf2_inc_pos(info, size);
      break;
    case DW_FORM_block2:
      dwarf2_iread_2(size, info);
      if (size > 0)
	dwarf2_inc_pos(info, size);
      break;
    case DW_FORM_block4:
      dwarf2_iread_4(size, info);
      if (size > 0)
	dwarf2_inc_pos(info, size);
      break;
    case DW_FORM_ref1:
    case DW_FORM_data1:
      dwarf2_inc_pos(info, 1);
      break;
    case DW_FORM_ref2:
    case DW_FORM_data2:
      dwarf2_inc_pos(info, 2);
      break;
    case DW_FORM_ref4:
    case DW_FORM_strp:
    case DW_FORM_data4:
      dwarf2_inc_pos(info, 4);
      break;
    case DW_FORM_ref8:
    case DW_FORM_data8:
      dwarf2_inc_pos(info, 8);
      break;
    case DW_FORM_string:
      dwarf2_inc_pos(info, strlen(dwarf2_ac_pos(info))+1);
      break;
    case DW_FORM_block:
      dwarf2_iuleb128(size, info);
      dwarf2_inc_pos(info, size);
      break;
    case DW_FORM_flag:
      dwarf2_inc_pos(info, 1);
      break;
    case DW_FORM_udata:
      dwarf2_iuleb128(size, info);
      break;
    case DW_FORM_sdata:
      dwarf2_ileb128(data, info);
      break;
    case DW_FORM_ref_udata:
      dwarf2_iuleb128(size, info);
      break;
    case DW_FORM_indirect:
      dwarf2_ileb128(data, info);
      form = data;
      goto fvstart;
      break;
    }

  NOPROFILER_ROUT(0);
}

/**
 * Parse a form value 
 * @param attr abbrev attribute to fill
 */
int 	   		edfmt_dwarf2_form_value(edfmtdw2abbattr_t *attr)
{
  unsigned int			i;
  char			*ptr;
  unsigned char 		addr[10]; /* 10 should be enough */
  unsigned int			data;
  long			ddata;

  NOPROFILER_IN();

  if (dwarf2_data(info) == NULL)
      NOPROFILER_ROUT(-1);

  /* Reset position on saved value */
  dwarf2_pos(info) = attr->infopos;

fvstart:

  /**
   * I will not explain every element because it will be too long
   * you can read dwarf2 documentation for more information.
   */
  switch (attr->form)
    {
    case DW_FORM_ref_addr:
      for (i = 0; i < current_cu->addr_size; i++)
	dwarf2_ipos(addr[i], info, unsigned char);

      attr->u.udata = *(unsigned long *) addr;
      break;
    case DW_FORM_addr:
      DW2_GETPTR(attr->u.vbuf, current_cu->addr_size, -1);
      for (i = 0; i < current_cu->addr_size; i++)
	dwarf2_ipos(attr->u.vbuf[i], info, unsigned char);
      break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
      switch (attr->form)
	{
	case DW_FORM_block1:
	  dwarf2_iread_1(attr->asize, info);
	  break;
	case DW_FORM_block2:
	  dwarf2_iread_2(attr->asize, info);
	  break;
	case DW_FORM_block4:
	  dwarf2_iread_4(attr->asize, info);
	  break;
	}

      if (attr->asize > 0)
	{
	  DW2_GETPTR(attr->u.vbuf, attr->asize, -1);
	  
	  for (i = 0; i < attr->asize; i++)
	    dwarf2_ipos(attr->u.vbuf[i], info, unsigned char);
	}
      break;
    case DW_FORM_data1:
      dwarf2_iread_1(ddata, info);
      attr->u.udata = (unsigned long) ddata & 0xFF;
      break;
    case DW_FORM_data2:
      dwarf2_iread_2(ddata, info);
      attr->u.udata = (unsigned long) ddata & 0xFFFF;
      break;
    case DW_FORM_data4:
      dwarf2_iread_4(ddata, info);
      attr->u.udata = (unsigned long) ddata & 0xFFFFFFFF;
      break;
    case DW_FORM_data8:
      dwarf2_iread_8(ddata, info);
      attr->u.udata = (unsigned long) ddata;
      break;
    case DW_FORM_string:
      attr->u.str = dwarf2_ac_pos(info);
      dwarf2_inc_pos(info, strlen(attr->u.str)+1);
      break;
    case DW_FORM_block:
      dwarf2_iuleb128(attr->asize, info);

      DW2_GETPTR(attr->u.vbuf, attr->asize, -1);

      for (i = 0; i < attr->asize; i++)
	dwarf2_ipos(attr->u.vbuf[i], info, char);
      break;
    case DW_FORM_flag:
      dwarf2_iread_1(ddata, info);
      attr->u.udata = (unsigned long) ddata & 0xFF;
      break;
    case DW_FORM_strp:
      ptr = (char *) dwarf2_data(str);
      dwarf2_iread_4(data, info);
      attr->u.str = (char *) ptr + data;
      break;
    case DW_FORM_udata:
      dwarf2_iuleb128(attr->u.udata, info);
      break;
    case DW_FORM_sdata:
      dwarf2_ileb128(attr->u.sdata, info);
      break;
    case DW_FORM_ref1:
      dwarf2_iread_1(attr->u.udata, info);
      break;
    case DW_FORM_ref2:
      dwarf2_iread_2(attr->u.udata, info);
      break;
    case DW_FORM_ref4:
      dwarf2_iread_4(attr->u.udata, info);
      break;
    case DW_FORM_ref8:
      dwarf2_iread_8(attr->u.udata, info);
      break;
    case DW_FORM_ref_udata:
      dwarf2_iuleb128(attr->u.udata, info);
      break;
    case DW_FORM_indirect:
      /* Read form from .debug_info */
      dwarf2_ileb128(data, info);
     
      /* Update attribute value and relaunch the function */
      attr->form = data;
      goto fvstart;
      break;
    }

  NOPROFILER_ROUT(0);
}


/**
 * Follow .debug_info form using abbrev_table as structure reference 
 * @param abbent abbrev entity to fill
 * @param pos info position
 */
int			edfmt_dwarf2_form(edfmtdw2abbent_t *abbent, unsigned int pos)
{  
  unsigned int			num_fetch, i;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have this section ? */
  if (dwarf2_data(abbrev) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 ".debug_info section not available", -1);

  dwarf2_pos(info) = pos;

  /* Retrieve abbrev number */
  dwarf2_iuleb128(num_fetch, info);

  /* We didn't use num_fetch == 0 */
  if (num_fetch == 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Retreive an abbrev element */
  if (edfmt_dwarf2_abbrev_read(abbent, num_fetch, pos) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Impossible to read the structure", -1);

  /* Resolv form value for each attribute */
  for (i = 0; abbent->attr[i].attr; i++)
    {
      switch(abbent->attr[i].attr)
	{
	  /* This tag inform for the next element (after childs) we have to get it */
	case DW_AT_sibling:
	  abbent->attr[i].infopos = dwarf2_pos(info);
	  edfmt_dwarf2_form_value(abbent->attr + i);
	  abbent->sib = current_cu->start_pos + abbent->attr[i].u.udata;
	  break;
	default:
	  /* All other elements are just iterate */
	  edfmt_dwarf2_form_it(abbent->attr + i);
	}
    }
  
  /* Update some position depending of the current status */
  if (abbent->children)
    abbent->child = dwarf2_pos(info);
  else if (abbent->sib <= 0)
    abbent->sib = dwarf2_pos(info);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * XXX: Re use it when it will be usefull
 * Read macinfo data 
 */
int			edfmt_dwarf2_mac(unsigned long offset)
{
  int			line = 0, file = 0;
  unsigned char      		type;
  unsigned int			i;
  char			*str = NULL;
  edfmtdw2macro_t 	*mac;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have this section ? */
  if (dwarf2_data(macinfo) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_macinfo section not available",  -1);

  /* Update position offset */
  dwarf2_pos(macinfo) = offset;

  do {
    dwarf2_iread_1(type, macinfo);

    switch(type)
      {
      case DW_MACINFO_define:
      case DW_MACINFO_undef:
	DW2_GETPTR(mac, sizeof(edfmtdw2macro_t), -1);

	mac->fileno = file;
	mac->def = type == DW_MACINFO_define ? 1 : 0;

	dwarf2_iuleb128(mac->line, macinfo);
	mac->str = dwarf2_ac_pos(macinfo);
	dwarf2_inc_pos(macinfo, strlen(mac->str)+1);

	/* Find the first space or ( */
	for (i = 0; i < DW2_MACRON_SIZE && mac->str[i]; i++)
	  if (mac->str[i] == ' ' || mac->str[i] == '(')
	    break;
	
	/* Copy the name */
	strncpy(mac->name, mac->str, i);
	mac->name[DW2_MACRON_SIZE - 1] = 0x00;

	/* Add macro into compil unit */
	/* TODO: Reuse when it will be usefull
	if (current_cu->macro == NULL)
	  current_cu->macro = mac;
	else
	  current_cu->last_macro->next = mac;

	current_cu->last_macro = mac;
	*/
	break;
      case DW_MACINFO_start_file:
	dwarf2_iuleb128(line, macinfo);
	dwarf2_iuleb128(file, macinfo);
	break;
      case DW_MACINFO_end_file:
	file = 0;
	line = 0;
	break;
      case DW_MACINFO_vendor_ext:
	dwarf2_iuleb128(line, macinfo);
	dwarf2_istr(str, macinfo);

	/* Add vendor parse engine here ? 
	   But I don't know any vendor extension so ... */
	break;
      }
  } while (type != 0 && dwarf2_pos(macinfo) < dwarf2_size(macinfo));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Parse a dwarf2 location, this location describe where an element is store 
 * @param loc information will be store on this structure
 * @param buf location is read from this buffer
 * @param size size of the buffer
 */
int	      		edfmt_dwarf2_loc(edfmtdw2loc_t *loc, unsigned char *buf, unsigned int size)
{
  unsigned int			i, spos, bsize, tmp_op;
  eresi_Addr		tmp_value;
  edfmtdw2loc_t		stack[75];
  int			bra;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0 || loc == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", -1);

  spos = 0;
  stack[spos].op = 0;
  stack[spos].value = 0;

  /* Iterate for each part of the block */
  for (i = 0; i < size; i++)
    {
      /* Push a stack operand */
      stack[++spos].op = buf[i];

      /**
       * I will not explain every element because it will be too long
       * you can read dwarf2 documentation for more information.
       */
      switch(buf[i])
	{
	case DW_OP_addr:
	  stack[spos].value = *(eresi_Addr *) (buf + (++i));
	  i += sizeof(eresi_Addr) - 1;
	  break;
	case DW_OP_deref:
	  stack[spos].op = DW_OP_addr;
	  if (stack[spos].value != NULL)
	    stack[spos].value = *(eresi_Addr *) stack[spos].value;
	  break;
	case DW_OP_const1u:
	  stack[spos].value = *(unsigned char *) (buf + (++i));
	  i++;
	  break;
	case DW_OP_const1s:
	  stack[spos].value = *(char *) (buf + (++i));
	  i++;
	  break;
	case DW_OP_const2u:
	  stack[spos].value = *(u_short *) (buf + (++i));
	  i += 2;
	  break;
	case DW_OP_const2s:
	  stack[spos].value = *(short *) (buf + (++i));
	  i += 2;
	  break;
	case DW_OP_const4u:
	  stack[spos].value = *(unsigned int *) (buf + (++i));
	  i += 4;
	  break;
	case DW_OP_const4s:
	  stack[spos].value = *(int *) (buf + (++i));
	  i += 4;
	  break;
	case DW_OP_const8u:
	  stack[spos].value = *(unsigned long *) (buf + (++i));
	  i += 8;
	  break;
	case DW_OP_const8s:	
	  stack[spos].value = *(long *) (buf + (++i));
	  i += 8;
	  break;
	case DW_OP_constu:	
	  stack[spos].value = edfmt_read_uleb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	case DW_OP_consts:	
	  stack[spos].value = edfmt_read_leb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	case DW_OP_dup:
	  stack[spos+1].op = stack[spos].op;
	  stack[spos+1].value = stack[spos].value;
	  spos++;
	  break;
	case DW_OP_drop:	
	  spos--;
	  break;
	case DW_OP_over:
	  stack[spos+1].op = stack[spos - 1].op;
	  stack[spos+1].value = stack[spos - 1].value;
	  spos++;
	  break;
	case DW_OP_pick:	
	  if (spos + buf[++i] < 0)
	    break;

	  stack[spos+1].op = stack[spos - buf[i]].op;
	  stack[spos+1].value = stack[spos - buf[i]].value;
	  spos++;
	  break;
	case DW_OP_swap:	
	  tmp_op = stack[spos].op;
	  tmp_value = stack[spos].value;
	  stack[spos].op = stack[spos - 1].op;
	  stack[spos].value = stack[spos - 1].value;
	  stack[spos - 1].op = tmp_op;
	  stack[spos - 1].value = tmp_value;
	  break;
	case DW_OP_rot:	
	  tmp_op = stack[spos].op;
	  tmp_value = stack[spos].value;
	  stack[spos].op = stack[spos - 1].op;
	  stack[spos].value = stack[spos - 1].value;
	  stack[spos - 1].op = tmp_op;
	  stack[spos - 1].value = tmp_value;

	  tmp_op = stack[spos+1].op;
	  tmp_value = stack[spos+1].value;
	  stack[spos+1].op = stack[spos - 2].op;
	  stack[spos+1].value = stack[spos - 2].value;
	  stack[spos - 2].op = tmp_op;
	  stack[spos - 2].value = tmp_value;
	  break;
	case DW_OP_abs:	
	  if (stack[spos].value < 0)
	    stack[spos].value = - stack[spos].value;
	  break;
	case DW_OP_and:	
	  stack[spos - 1].value &= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_div:	
	  stack[spos - 1].value /= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_minus:	
	  stack[spos - 1].value -= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_mod:	
	  stack[spos - 1].value = stack[spos - 1].value % stack[spos].value;
	  spos--;
	  break;
	case DW_OP_mul:	
	  stack[spos - 1].value *= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_neg:	
	  stack[spos].value = -stack[spos].value;
	  break;
	case DW_OP_not:	
	  stack[spos].value = !stack[spos].value;
	  break;
	case DW_OP_or:	
	  stack[spos - 1].value |= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_plus:	
	  stack[spos - 1].value += stack[spos].value;
	  spos--;
	  break;
	case DW_OP_plus_uconst:	
	  stack[spos].value += edfmt_read_uleb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	case DW_OP_shl:	
	  stack[spos - 1].value <<= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_shr:	
	  stack[spos - 1].value >>= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_xor:	
	  stack[spos - 1].value ^= stack[spos].value;
	  spos--;
	  break;
	case DW_OP_bra:
	  bra = *(short *) (buf + (++i));
	  i += 2;

	  if (stack[spos].value != 0)
	    i += bra;
	  spos--;
	  break;
	case DW_OP_eq:	
	  stack[spos - 1].value = (stack[spos].value == stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_ge:
	  stack[spos - 1].value = (stack[spos].value >= stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_gt:	
	  stack[spos - 1].value = (stack[spos].value > stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_le:	
	  stack[spos - 1].value = (stack[spos].value <= stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_lt:	
	  stack[spos - 1].value = (stack[spos].value < stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_ne:	
	  stack[spos - 1].value = (stack[spos].value != stack[spos - 1].value);
	  spos--;
	  break;
	case DW_OP_skip:	
	  bra = *(short *) (buf + (++i));
	  i += 2;
	  i += bra;
	  break;
	case DW_OP_lit0:	
	case DW_OP_lit1:	
	case DW_OP_lit2:	
	case DW_OP_lit3:	
	case DW_OP_lit4:	
	case DW_OP_lit5:	
	case DW_OP_lit6:	
	case DW_OP_lit7:	
	case DW_OP_lit8:	
	case DW_OP_lit9:	
	case DW_OP_lit10:	
	case DW_OP_lit11:	
	case DW_OP_lit12:	
	case DW_OP_lit13:	
	case DW_OP_lit14:	
	case DW_OP_lit15:	
	case DW_OP_lit16:	
	case DW_OP_lit17:	
	case DW_OP_lit18:	
	case DW_OP_lit19:	
	case DW_OP_lit20:	
	case DW_OP_lit21:	
	case DW_OP_lit22:	
	case DW_OP_lit23:	
	case DW_OP_lit24:	
	case DW_OP_lit25:	
	case DW_OP_lit26:	
	case DW_OP_lit27:	
	case DW_OP_lit28:	
	case DW_OP_lit29:	
	case DW_OP_lit30:	
	case DW_OP_lit31:	
	  stack[spos].value = buf[i] - DW_OP_lit0;
	  break;
	case DW_OP_reg0:	
	case DW_OP_reg1:	
	case DW_OP_reg2:	
	case DW_OP_reg3:	
	case DW_OP_reg4:	
	case DW_OP_reg5:	
	case DW_OP_reg6:	
	case DW_OP_reg7:	
	case DW_OP_reg8:	
	case DW_OP_reg9:	
	case DW_OP_reg10:	
	case DW_OP_reg11:	
	case DW_OP_reg12:	
	case DW_OP_reg13:	
	case DW_OP_reg14:	
	case DW_OP_reg15:	
	case DW_OP_reg16:	
	case DW_OP_reg17:	
	case DW_OP_reg18:	
	case DW_OP_reg19:	
	case DW_OP_reg20:	
	case DW_OP_reg21:	
	case DW_OP_reg22:	
	case DW_OP_reg23:	
	case DW_OP_reg24:	
	case DW_OP_reg25:	
	case DW_OP_reg26:	
	case DW_OP_reg27:	
	case DW_OP_reg28:	
	case DW_OP_reg29:	
	case DW_OP_reg30:	
	case DW_OP_reg31:	
	  stack[spos].value = buf[i] - DW_OP_reg0;
	  break;
	case DW_OP_breg0:	
	case DW_OP_breg1:	
	case DW_OP_breg2:	
	case DW_OP_breg3:	
	case DW_OP_breg4:	
	case DW_OP_breg5:	
	case DW_OP_breg6:	
	case DW_OP_breg7:	
	case DW_OP_breg8:	
	case DW_OP_breg9:	
	case DW_OP_breg10:	
	case DW_OP_breg11:	
	case DW_OP_breg12:	
	case DW_OP_breg13:	
	case DW_OP_breg14:	
	case DW_OP_breg15:	
	case DW_OP_breg16:	
	case DW_OP_breg17:	
	case DW_OP_breg18:	
	case DW_OP_breg19:	
	case DW_OP_breg20:	
	case DW_OP_breg21:	
	case DW_OP_breg22:	
	case DW_OP_breg23:	
	case DW_OP_breg24:	
	case DW_OP_breg25:	
	case DW_OP_breg26:	
	case DW_OP_breg27:	
	case DW_OP_breg28:	
	case DW_OP_breg29:	
	case DW_OP_breg30:	
	case DW_OP_breg31:	
	  stack[spos].value = buf[i] - DW_OP_breg0;
	  break;
	case DW_OP_regx:
	  stack[spos].value = edfmt_read_uleb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	case DW_OP_fbreg:
	  stack[spos].value = edfmt_read_leb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	case DW_OP_bregx:	
	  stack[spos].value = edfmt_read_uleb128(buf + (++i), &bsize);
	  i += bsize;
	  break;
	  /*
	    TODO: Find how we can parse that correctly
	case DW_OP_deref_size:	
	  stack[spos].op = DW_OP_addr;
	  tmp_value = stack[spos].value;
	  stack[spos].value = 0;
	  for (z = 0, i++; z < buf[i]; z++)
	    stack[spos].value = ((char*)&tmp_value)[z];
	  break;
	  */
	case DW_OP_nop:	
	  break;
	case DW_OP_shra:	
	case DW_OP_piece:	
	case DW_OP_xderef_size:	
	case DW_OP_xderef:
	  /* Unimplemented, for multiple addresse space architecture ? */
	  break;
	}      
    }

  /* Save location value */
  loc->op = stack[spos].op;
  loc->value = stack[spos].value;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * XXX: Re use it when it will be usefull
 * Parse data part of .debug_line format 
 */
static int		edfmt_dwarf2_line_data(edfmtdw2linehead_t *header)
{
  eresi_Addr  		addr;
  unsigned int			file;
  unsigned int			line;
  unsigned int			column;
  unsigned int	      		is_stmt;
  unsigned int	       		basic_block;
  unsigned int			end_sequence;
  unsigned long		tmpf;

  unsigned int			i, bsize;
  unsigned char       		opc, ext_opc;
  char			*read_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (header == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);

  /* Loop until end position */
  while (dwarf2_pos(line) < header->end_pos)
    {
      addr = 0;
      file = 1;
      line = 1;
      column = 0;
      is_stmt = header->default_stmt;
      basic_block = 0;
      end_sequence = 0;

      while (!end_sequence)
	{
	  dwarf2_iread_1(opc, line);

	  if (opc >= header->opcode_base)
	    {
	      opc -= header->opcode_base;
	      line += header->line_base + (opc % header->line_range);
	      addr += (opc / header->line_range) * header->min_length_instr;

	      edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
	      basic_block = 1;
	    }
	  else
	    {
	      switch(opc)
		{
		case DW_LNS_extended_op:
		  dwarf2_iuleb128(tmpf, line);

		  dwarf2_iread_1(ext_opc, line);
		  switch(ext_opc)
		    {
		    case DW_LNE_end_sequence:
		      end_sequence = 1;
		      edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		      break;
		    case DW_LNE_set_address:
		      DW2_GETPTR(read_addr, current_cu->addr_size, -1);
		      
		      for (i = 0; i < current_cu->addr_size; i++)
			dwarf2_ipos(read_addr[i], line, unsigned char);

		      addr = *(eresi_Addr *) read_addr;
		      break;
		    case DW_LNE_define_file:
		      header->files_number++;
		      /* Realloc pointers */
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__,header->files_name, header->files_name, 
			       sizeof(char*)*header->files_number, -1);
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__,header->files_dindex, header->files_dindex, 
			       sizeof(unsigned int)*header->files_number, -1);
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__,header->files_time, header->files_time, 
			       sizeof(unsigned int)*header->files_number, -1);
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__,header->files_len, header->files_len, 
			       sizeof(unsigned int)*header->files_number, -1);

		      /* Read information and fill new entrie */
		      dwarf2_istr(header->files_name[header->files_number], line);
		      dwarf2_iuleb128(header->files_dindex[header->files_number], line);
		      dwarf2_iuleb128(header->files_time[header->files_number], line);
		      dwarf2_iuleb128(header->files_len[header->files_number], line);
		      break;
		    }
		  break;
		case DW_LNS_copy:
		  basic_block = 0;
		  edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		  break;
		case DW_LNS_advance_pc:
		  dwarf2_iuleb128(tmpf, line);
		  addr += tmpf * header->min_length_instr;
		  break;
		case DW_LNS_advance_line:
		  dwarf2_ileb128(tmpf, line);
		  line += tmpf;
		  break;
		case DW_LNS_set_file:
		  dwarf2_iuleb128(file, line);
		  break;
		case DW_LNS_set_column:
		  dwarf2_iuleb128(column, line);
		  break;
		case DW_LNS_negate_stmt:
		  is_stmt = !is_stmt;
		  break;
		case DW_LNS_set_basic_block:
		  basic_block = 1;
		  break;
		case DW_LNS_const_add_pc:
		  addr += ((255 - header->opcode_base) / header->line_range) 
		    * header->min_length_instr;
		  break;
		case DW_LNS_fixed_advance_pc:
		  dwarf2_iread_2(bsize, line);
		  addr += bsize;
		  break;
		default:
		  for (i = 0; i < header->std_opcode_length[opc]; i++)
		    dwarf2_iuleb128(tmpf, line);
		}
	    }
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * XXX: Re use it when it will be usefull
 * Parse .debug_line 
 */
int			edfmt_dwarf2_line(unsigned long offset)
{
  unsigned int			i;
  edfmtdw2linehead_t 	header;
  unsigned long      		prev_pos, tmpf;
  char			*strf;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Update position offset */
  dwarf2_pos(line) = offset;

  /* Parse the header */
  dwarf2_iread_4(header.total_length, line);
  header.end_pos = dwarf2_pos(line) + header.total_length;
  dwarf2_iread_2(header.version, line);
  dwarf2_iread_4(header.prologue_length, line);
  header.prologue_pos = dwarf2_pos(line);
  dwarf2_iread_1(header.min_length_instr, line);
  dwarf2_iread_1(header.default_stmt, line);
  dwarf2_iread_1(header.line_base, line);
  dwarf2_iread_1(header.line_range, line);
  dwarf2_iread_1(header.opcode_base, line);
  header.std_opcode_length = dwarf2_ac_pos(line);
  dwarf2_inc_pos(line, header.opcode_base - 1);
  
  /* Dir first pass */
  prev_pos = dwarf2_pos(line);
  for (i = 0; (dwarf2_ac_pos(line))[0] != '\0'; i++)
    dwarf2_inc_pos(line, strlen(dwarf2_ac_pos(line))+1);
  dwarf2_pos(line) = prev_pos;

  DW2_GETPTR(header.dirs, sizeof(char*)*i, -1);
  header.dirs_number = i+1;

  /* Dir second pass */
  for (i = 0; (dwarf2_ac_pos(line))[0] != '\0'; i++)
    {
      header.dirs[i] = dwarf2_ac_pos(line);
      dwarf2_inc_pos(line, strlen(dwarf2_ac_pos(line))+1);
    }
  dwarf2_inc_pos(line, 1);

  /* Filename first pass */
  prev_pos = dwarf2_pos(line);
  for (i = 0; (dwarf2_ac_pos(line))[0] != '\0'; i++)
    {
      dwarf2_istr(strf, line);
      dwarf2_iuleb128(tmpf, line);
      dwarf2_iuleb128(tmpf, line);
      dwarf2_iuleb128(tmpf, line);
    }
  dwarf2_pos(line) = prev_pos;

  DW2_GETPTR(header.files_name, sizeof(char*)*i, -1);
  DW2_GETPTR(header.files_dindex, sizeof(unsigned int)*i, -1);
  DW2_GETPTR(header.files_time, sizeof(unsigned int)*i, -1);
  DW2_GETPTR(header.files_len, sizeof(unsigned int)*i, -1);

  header.files_number = i+1;

  /* Filename second pass */
  for (i = 0; (dwarf2_ac_pos(line))[0] != '\0'; i++)
    {
      dwarf2_istr(header.files_name[i], line);
      dwarf2_iuleb128(header.files_dindex[i], line);
      dwarf2_iuleb128(header.files_time[i], line);
      dwarf2_iuleb128(header.files_len[i], line);
    }
  dwarf2_inc_pos(line, 1);

  /* Check position validity */
  if (header.prologue_pos + header.prologue_length != dwarf2_pos(line))
    dwarf2_pos(line) = header.prologue_pos + header.prologue_length;

  /* Save files & directories list */
  current_cu->dirs 	   = header.dirs;
  current_cu->files_name   = header.files_name;
  current_cu->files_dindex = header.files_dindex;
  current_cu->files_time   = header.files_time;
  current_cu->files_len    = header.files_len;
  current_cu->files_number = header.files_number;

  /* Parse the rest */
  edfmt_dwarf2_line_data(&header);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * XXX: Re use it when it will be usefull
 * Record a new line entry
 */
int			edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, unsigned int line, unsigned int column, 
					      eresi_Addr addr, unsigned int fid)
{
  edfmtdw2line_t 	*pline;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  DW2_GETPTR(pline, sizeof(edfmtdw2line_t), -1);

  pline->addr = addr;
  pline->line = line;
  pline->column = column;
  pline->fileid = fid;
  pline->cu = cu;

  /* Add line informations */
  /* TODO: Reuse when it will be usefull 
  if (cu->line == NULL)
    cu->line = pline;
  else
    cu->last_line->next = pline;

  cu->last_line = pline;
  */
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
