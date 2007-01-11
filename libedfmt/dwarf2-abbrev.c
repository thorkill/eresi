/*
** dwarf2-abbrev.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

hash_t abbrev_table;

/* Create a new reference that wait for resolution after compil unit or global parsing */
static int	edfmt_dwarf2_newfref(long offset, edfmtdw2abbattr_t *attr, u_char global)
{
  edfmtdw2fref_t *tmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (attr == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);  

  XALLOC(tmp, sizeof(edfmtdw2fref_t), -1);

  tmp->offset = offset;
  tmp->attr = attr;

  if (global)
    {
      tmp->next = ref_global;
      ref_global = tmp;
    }
  else
    {
      tmp->next = ref_cu;
      ref_cu = tmp;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/* Lookup on abbrev_table */
edfmtdw2abbent_t *edfmt_dwarf2_lookup_abbrev(u_int num_fetch)
{
  char		ckey[DW2_CKEY_SIZE];
  edfmtdw2abbent_t *ent;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  edfmt_dwarf2_ckey(ckey, DW2_CKEY_SIZE, num_fetch);
  ent = (edfmtdw2abbent_t *) hash_get(&abbrev_table, ckey);

  if (ent == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Abbrev number not found", NULL);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ent);
}

/* Parse the .debug_abbrev format */
int		edfmt_dwarf2_abbrev()
{
  u_int		num;
  edfmtdw2abbent_t *ent = NULL;
  edfmtdw2abbent_t *tmp;
  edfmtdw2abbattr_t *attr = NULL;
  u_int		allocattr = 0;
  u_int		attri = 0;
  u_int		level = 1;
  const	u_int	allocstep = 12;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have this section ? */
  if (dw2_sections.abbrev.data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_abbrev section not available",  -1);

  do {
    /* Fetch the key number */
    ci_uleb128(num, abbrev);
    
    /* A level end */
    if (num == 0)
      {
	level--;
	/* It's the end ! */
	if (level == 0)
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	continue;
      }
    
    /* Save last element */
    tmp = ent;
    
    /* Alloc next elements */
    XALLOC(ent, sizeof(edfmtdw2abbent_t), -1);
    
    /* Fill main data */
    ent->key = num;
    ci_uleb128(ent->tag, abbrev);
    ci_read_1(ent->children, abbrev);
    
    attri = allocattr = 0;
    attr = NULL;
    
    /* Update level */
    ent->level = level;
    
    if (ent->children != 0)
      level++;
    
    /* Update links between entities */
    if (tmp)
      {
	/* Direct child */
	if (ent->level > tmp->level)
	  {
	    ent->parent = tmp;
	  }
	/* Next sibling of last element parent */
	else if (ent->level < tmp->level)
	  {
	    if (tmp->parent != NULL)
	      {
		ent->parent = tmp->parent->parent;
		tmp->parent->sib = ent;
	      }
	  }
	/* Next sibling */
	else
	  {
	    tmp->sib = ent;
	    ent->parent = tmp->parent;
	  }
      }
    
    /* Parse attributes */
    do {
      /* First allocation */
      if (attr == NULL)
	{
	  allocattr = allocstep;
	  XALLOC(attr, sizeof(edfmtdw2abbattr_t)*allocattr, -1);
	}
      
      /* We fill all the array, so we realloc all */
      if (attri == allocattr)
	{
	  allocattr += allocstep;
	  XREALLOC(attr, attr, sizeof(edfmtdw2abbattr_t)*allocattr, -1);
	}

      /* Fill attribute structure */
      ci_uleb128(attr[attri].attr, abbrev);
      ci_uleb128(attr[attri].form, abbrev);
    } while (attr[attri++].attr);
      
    ent->attr = attr;
    
    /* Add into abbrev table */
    edfmt_dwarf2_ckey(ent->ckey, DW2_CKEY_SIZE - 1, ent->key);
    hash_add(&abbrev_table, ent->ckey, (void *) ent);
  } while (dw2_sections.abbrev.pos < dw2_sections.abbrev.size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse a form value */
static int    	edfmt_dwarf2_form_value(edfmtdw2abbattr_t *attr)
{
  u_int		i;
  u_char 	*addr;
  u_int		data;
  u_long	ldata;
  long		ddata;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (attr->form)
    {
    case DW_FORM_ref_addr:
      XALLOC(addr, current_cu->addr_size, -1);

      for (i = 0; i < current_cu->addr_size; i++)
	ci_pos(addr[i], info, u_char);

      edfmt_dwarf2_newfref(*(u_int *) addr, attr, 1);
      XFREE(addr);
      break;
    case DW_FORM_addr:
      XALLOC(attr->u.vbuf, current_cu->addr_size, -1);
      for (i = 0; i < current_cu->addr_size; i++)
	ci_pos(attr->u.vbuf[i], info, u_char);
      break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
      switch (attr->form)
	{
	case DW_FORM_block1:
	  ci_read_1(attr->asize, info);
	  break;
	case DW_FORM_block2:
	  ci_read_2(attr->asize, info);
	  break;
	case DW_FORM_block4:
	  ci_read_4(attr->asize, info);
	  break;
	}

      XALLOC(attr->u.vbuf, attr->asize, -1);

      for (i = 0; i < attr->asize; i++)
	ci_pos(attr->u.vbuf[i], info, u_char);
      break;
    case DW_FORM_data1:
      ci_read_1(attr->u.udata, info);
      break;
    case DW_FORM_data2:
      ci_read_2(attr->u.udata, info);
      break;
    case DW_FORM_data4:
      ci_read_4(attr->u.udata, info);
      break;
    case DW_FORM_data8:
      ci_read_8(attr->u.udata, info);
      break;
    case DW_FORM_string:
      attr->u.str = ac_pos(info);
      inc_pos(info, strlen(attr->u.str)+1);
      break;
    case DW_FORM_block:
      ci_uleb128(attr->asize, info);

      XALLOC(attr->u.vbuf, attr->asize, -1);

      for (i = 0; i < attr->asize; i++)
	ci_pos(attr->u.vbuf[i], info, char);
      break;
    case DW_FORM_flag:
      ci_read_1(attr->u.udata, info);
      break;
    case DW_FORM_strp:
      addr = (char *) dw2_sections.str.data;
      ci_read_4(data, info);
      attr->u.str = addr + data;
      break;
    case DW_FORM_udata:
      ci_uleb128(attr->u.udata, info);
      break;
    case DW_FORM_sdata:
      ci_leb128(attr->u.sdata, info);
      break;
    case DW_FORM_ref1:
      ci_read_1(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref2:
      ci_read_2(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref4:
      ci_read_4(data, info);
      edfmt_dwarf2_newfref(data, attr, 0);
      break;
    case DW_FORM_ref8:
      ci_read_8(ddata, info);
      edfmt_dwarf2_newfref(ddata, attr, 0);
      break;
    case DW_FORM_ref_udata:
      ci_uleb128(ldata, info);

      edfmt_dwarf2_newfref(ldata, attr, 0);
      break;
    case DW_FORM_indirect:
      /* Read form from .debug_info */
      ci_leb128(data, info);
     
      /* Update attribute value and relaunch the function */
      attr->form = data;
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 edfmt_dwarf2_form_value(attr));
      break;
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown form", -1);
      break;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Follow .debug_info form using abbrev_table as structure reference */
int		edfmt_dwarf2_form(u_int endpos)
{  
  char		*ref_global_ckey;
  char	      	*ref_cu_ckey;
  u_int		num_fetch, i, level = 1;
  edfmtdw2abbent_t *ent;
  u_int		start_pos;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have this section ? */
  if (dw2_sections.abbrev.data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_info section not available",  -1);

  do {
    start_pos = dw2_sections.info.pos;
    
    /* Retrieve abbrev number */
    ci_uleb128(num_fetch, info);

    /* Update current level */
    if (num_fetch == 0)
      {
	level--;
	continue;
      }
    
    ent = edfmt_dwarf2_lookup_abbrev(num_fetch);
    if (ent == NULL)
      continue;

    /* Update references */
    XALLOC(ref_global_ckey, DW2_CKEY_SIZE, -1);
    XALLOC(ref_cu_ckey, DW2_CKEY_SIZE, -1);

    edfmt_dwarf2_ckey(ref_global_ckey, DW2_CKEY_SIZE, start_pos);
    edfmt_dwarf2_ckey(ref_cu_ckey, DW2_CKEY_SIZE, start_pos - current_cu->start_pos);

    hash_add(&ref_global_table, ref_global_ckey, (void *) ent);
    hash_add(&ref_cu_table, ref_cu_ckey, (void *) ent);
    
    /* If we enter into a new child list, increase the level */
    if (ent->children != 0)
      level++;

    /* Resolv form value for each attribute */
    for (i = 0; ent->attr[i].attr; i++)
      {
	edfmt_dwarf2_form_value(ent->attr + i);

	switch(ent->attr[i].attr)
	  {
	  case DW_AT_stmt_list:
	    edfmt_dwarf2_line(ent->attr[i].u.udata);
	    break;
	  case DW_AT_location:
	    edfmt_dwarf2_loc(&ent->attr[i].loc, ent->attr[i].u.vbuf, ent->attr[i].asize);
	    break;
	  case DW_AT_macro_info:
	    edfmt_dwarf2_mac(ent->attr[i].u.udata);
	    break;
	  }
      }
  } while(level > 0 && dw2_sections.info.pos < endpos);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Read macinfo data */
int		edfmt_dwarf2_mac(u_long offset)
{
  int		line = 0, file = 0;
  u_char      	type;
  u_int		i;
  char		*str = NULL;
  edfmtdw2macro_t *mac;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do we have this section ? */
  if (dw2_sections.macinfo.data == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_macinfo section not available",  -1);

  // Update position offset
  dw2_sections.macinfo.pos = offset;

  do {
    ci_read_1(type, macinfo);

    switch(type)
      {
      case DW_MACINFO_define:
      case DW_MACINFO_undef:
	XALLOC(mac, sizeof(edfmtdw2macro_t), -1);

	mac->fileno = file;
	mac->def = type == DW_MACINFO_define ? 1 : 0;

	ci_uleb128(mac->line, macinfo);
	mac->str = ac_pos(macinfo);
	inc_pos(macinfo, strlen(mac->str)+1);

	/* Find the first space or ( */
	for (i = 0; i < DW2_MACRON_SIZE && mac->str[i]; i++)
	  if (mac->str[i] == ' ' || mac->str[i] == '(')
	    break;
	
	/* Copy the name */
	strncpy(mac->name, mac->str, i);
	mac->name[DW2_MACRON_SIZE - 1] = 0x00;
	
	/* Add on local and global scope */
	hash_add(&debug_info.macros, mac->name, (void *) mac);
	hash_add(&current_cu->macros, mac->name, (void *) mac);
	break;
      case DW_MACINFO_start_file:
	ci_uleb128(line, macinfo);
	ci_uleb128(file, macinfo);
	break;
      case DW_MACINFO_end_file:
	file = 0;
	line = 0;
	break;
      case DW_MACINFO_vendor_ext:
	ci_uleb128(line, macinfo);
	ci_str(str, macinfo);

	/* Add vendor parse engine here ? 
	   But I don't know any vendor extension so ... */
	break;
      }
  } while (type != 0 && dw2_sections.macinfo.pos < dw2_sections.macinfo.size);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int	      	edfmt_dwarf2_loc(edfmtdw2loc_t *loc, char *buf, u_int size)
{
  u_int		i, z, spos, bsize, tmp_op;
  elfsh_Addr	tmp_value;
  edfmtdw2loc_t	stack[75];
  int		bra;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0 || loc == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid paramters", -1);

  spos = 0;
  stack[spos].op = 0;
  stack[spos].value = 0;

  /* Iterate for each part of the block */
  for (i = 0; i < size; i++)
    {
      /* Push a stack operand */
      switch(buf[i] & 0xFF)
	{
	case DW_OP_addr:
	case DW_OP_const1u:
	case DW_OP_const1s:
	case DW_OP_const2u:
	case DW_OP_const2s:
	case DW_OP_const4u:
	case DW_OP_const4s:
	case DW_OP_const8u:
	case DW_OP_const8s:	
	case DW_OP_constu:	
	case DW_OP_consts:	
	case DW_OP_regx:
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
	case DW_OP_bregx:
	case DW_OP_fbreg:	
	  stack[++spos].op = buf[i];
	  break;
	}

      /* Thread value */
      switch(buf[i] & 0xFF)
	{
	case DW_OP_addr:
	  stack[spos].value = *(elfsh_Addr *) (buf + (++i));
	  i += sizeof(elfsh_Addr) - 1;
	  break;
	case DW_OP_deref:
	  stack[spos].op = DW_OP_addr;
	  break;
	case DW_OP_const1u:
	  stack[spos].value = *(u_char *) (buf + (++i));
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
	  stack[spos].value = *(u_int *) (buf + (++i));
	  i += 4;
	  break;
	case DW_OP_const4s:
	  stack[spos].value = *(int *) (buf + (++i));
	  i += 4;
	  break;
	case DW_OP_const8u:
	  stack[spos].value = *(u_long *) (buf + (++i));
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
	case DW_OP_deref_size:	
	  stack[spos].op = DW_OP_addr;
	  tmp_value = stack[spos].value;
	  stack[spos].value = 0;
	  for (z = 0; z < buf[i+1]; z++)
	    stack[spos].value = ((char*)&tmp_value)[z];
	  break;
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse data part of .debug_line format */
static int	edfmt_dwarf2_line_data(edfmtdw2linehead_t *header)
{
  elfsh_Addr  	addr;
  u_int		file;
  u_int		line;
  u_int		column;
  u_int	      	is_stmt;
  u_int		basic_block;
  u_int		end_sequence;
  u_long	tmpf;

  u_int		i, bsize;
  u_char       	opc, ext_opc;
  char		*read_addr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (header == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", -1);

  /* Loop until end position */
  while (dw2_sections.line.pos < header->end_pos)
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
	  ci_read_1(opc, line);

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
		  ci_uleb128(tmpf, line);

		  ci_read_1(ext_opc, line);
		  switch(ext_opc)
		    {
		    case DW_LNE_end_sequence:
		      end_sequence = 1;
		      edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		      break;
		    case DW_LNE_set_address:
		      XALLOC(read_addr, current_cu->addr_size, -1);
		      
		      for (i = 0; i < current_cu->addr_size; i++)
			ci_pos(read_addr[i], line, u_char);

		      addr = *(elfsh_Addr *) read_addr;
		      
		      XFREE(read_addr);
		      break;
		    case DW_LNE_define_file:
		      header->files_number++;
		      /* Realloc pointers */
		      XREALLOC(header->files_name, header->files_name, 
			       sizeof(char*)*header->files_number, -1);
		      XREALLOC(header->files_dindex, header->files_dindex, 
			       sizeof(u_int)*header->files_number, -1);
		      XREALLOC(header->files_time, header->files_time, 
			       sizeof(u_int)*header->files_number, -1);
		      XREALLOC(header->files_len, header->files_len, 
			       sizeof(u_int)*header->files_number, -1);

		      /* Read information and fill new entrie */
		      ci_str(header->files_name[header->files_number], line);
		      ci_uleb128(header->files_dindex[header->files_number], line);
		      ci_uleb128(header->files_time[header->files_number], line);
		      ci_uleb128(header->files_len[header->files_number], line);
		      break;
		    }
		  break;
		case DW_LNS_copy:
		  basic_block = 0;
		  edfmt_dwarf2_line_rec(current_cu, line, column, addr, file - 1);
		  break;
		case DW_LNS_advance_pc:
		  ci_uleb128(tmpf, line);
		  addr += tmpf * header->min_length_instr;
		  break;
		case DW_LNS_advance_line:
		  ci_leb128(tmpf, line);
		  line += tmpf;
		  break;
		case DW_LNS_set_file:
		  ci_uleb128(file, line);
		  break;
		case DW_LNS_set_column:
		  ci_uleb128(column, line);
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
		  ci_read_2(bsize, line);
		  addr += bsize;
		  break;
		default:
		  for (i = 0; i < header->std_opcode_length[opc]; i++)
		    ci_uleb128(tmpf, line);
		}
	    }
	}
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse .debug_line */
int		edfmt_dwarf2_line(u_long offset)
{
  u_int		i;
  edfmtdw2linehead_t header;
  u_long      	prev_pos, tmpf;
  char		*strf;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Update position offset */
  dw2_sections.line.pos = offset;

  /* Parse the header */
  ci_read_4(header.total_length, line);
  header.end_pos = dw2_sections.line.pos + header.total_length;
  ci_read_2(header.version, line);
  ci_read_4(header.prologue_length, line);
  header.prologue_pos = dw2_sections.line.pos;
  ci_read_1(header.min_length_instr, line);
  ci_read_1(header.default_stmt, line);
  ci_read_1(header.line_base, line);
  ci_read_1(header.line_range, line);
  ci_read_1(header.opcode_base, line);
  header.std_opcode_length = ac_pos(line);
  inc_pos(line, header.opcode_base - 1);
  
  /* Dir first pass */
  prev_pos = dw2_sections.line.pos;
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    inc_pos(line, strlen(ac_pos(line))+1);
  dw2_sections.line.pos = prev_pos;

  XALLOC(header.dirs, sizeof(char*)*i, -1);
  header.dirs_number = i;

  /* Dir second pass */
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      header.dirs[i] = ac_pos(line);
      inc_pos(line, strlen(ac_pos(line))+1);
    }
  inc_pos(line, 1);

  /* Filename first pass */
  prev_pos = dw2_sections.line.pos;
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      ci_str(strf, line);
      ci_uleb128(tmpf, line);
      ci_uleb128(tmpf, line);
      ci_uleb128(tmpf, line);
    }
  dw2_sections.line.pos = prev_pos;

  XALLOC(header.files_name, sizeof(char*)*i, -1);
  XALLOC(header.files_dindex, sizeof(u_int)*i, -1);
  XALLOC(header.files_time, sizeof(u_int)*i, -1);
  XALLOC(header.files_len, sizeof(u_int)*i, -1);
  header.files_number = i;

  /* Filename second pass */
  for (i = 0; (ac_pos(line))[0] != '\0'; i++)
    {
      ci_str(header.files_name[i], line);
      ci_uleb128(header.files_dindex[i], line);
      ci_uleb128(header.files_time[i], line);
      ci_uleb128(header.files_len[i], line);
    }
  inc_pos(line, 1);

  /* Check position validity */
  if (header.prologue_pos + header.prologue_length != dw2_sections.line.pos)
    dw2_sections.line.pos = header.prologue_pos + header.prologue_length;

  /* Save files & directories list */
  current_cu->dirs 	   = header.dirs;
  current_cu->files_name   = header.files_name;
  current_cu->files_dindex = header.files_dindex;
  current_cu->files_time   = header.files_time;
  current_cu->files_len    = header.files_len;

  /* Parse the rest */
  edfmt_dwarf2_line_data(&header);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		edfmt_dwarf2_line_rec(edfmtdw2cu_t *cu, u_int line, u_int column, 
				      elfsh_Addr addr, u_int fid)
{
  edfmtdw2line_t *pline;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(pline, sizeof(edfmtdw2line_t), -1);

  pline->addr = addr;
  pline->line = line;
  pline->column = column;
  pline->fileid = fid;
  pline->cu = cu;

  /* Create hash names */
  edfmt_dwarf2_cline(pline->cline, DW2_CLINE_SIZE - 1, pline->line, cu->files_name[fid]);
  edfmt_dwarf2_caddr(pline->caddr, DW2_CADDR_SIZE - 1, pline->addr);

  /* Global */
  hash_add(&debug_info.file_line, pline->cline, (void *) pline);
  hash_add(&debug_info.addr, pline->caddr, (void *) pline);

  /* Local */
  hash_add(&cu->file_line, pline->cline, (void *) pline);
  hash_add(&cu->addr, pline->caddr, (void *) pline);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
