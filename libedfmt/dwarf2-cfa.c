/*
** dwarf2-cfa.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

/* Submit a new row state */
static int	edfmt_dwarf2_cfa_newstate(edfmtdw2cfastate_t *reg)
{
  edfmtdw2cfastate_t *tmp;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (reg == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);  

  XALLOC(tmp, sizeof(edfmtdw2cfastate_t), -1);
  memcpy(tmp, reg, sizeof(edfmtdw2cfastate_t));
  
  edfmt_dwarf2_caddr(tmp->caddr, DW2_CADDR_SIZE, tmp->addr);
  
  /* Add local and gloabal */
  hash_add(&current_cu->cfa, tmp->caddr, (void *) tmp);
  hash_add(&debug_info.cfa, tmp->caddr, (void *) tmp);
      
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/* Parse .debug_frame FDE + data */
static int	edfmt_dwarf2_cfa_data(edfmtdw2cfastate_t *state, 
				      edfmtdw2cfahead_t *header, u_char init)
{
  u_char	cfa, cfa_data, scfa, c_low, c_high;
  int		data, reg;
  elfsh_Addr	nloc;
  edfmtdw2cfastate_t save_stack[20];
  int		isave_pos = 0;
  int      	cie;
  int      	length, addr_range, addr_start;
  u_long       	last_pos;
  u_long	max_pos;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!init)
    {
      *state = header->init;
    }
  else
    {
      /* Reset current state except location */
      nloc = state->addr;
      memset(state, 0x00, sizeof(edfmtdw2cfastate_t));
      state->addr = nloc;
      nloc = 0;
    }

  /* We read FDE if we aren't on init state */
  if (!init)
    {
      ci_read_4(length, frame);	       	// Length (without length)
      last_pos = dw2_sections.frame.pos;
      ci_read_4(cie, frame);		// Cie ptr
      ci_read_4(addr_start, frame); 	// Initial location
      ci_read_4(addr_range, frame);	// Address range
      
      state->addr = addr_start;      
    }
  else
    {
      last_pos = length = addr_range = addr_start = 0;
    }

  /* Max position (end of while loop) */
  max_pos = init ? header->end_offset : last_pos + length;
  
  do {
    ci_read_1(cfa, frame);
    scfa = cfa;

    /* Find low and high parts of the id */
    c_low = (cfa >> 6) & 0xFF;
    c_high = (cfa & (~(0xc0))) & 0xFF;

    /* Dispatch */
    cfa = c_low ? cfa - c_high : c_high;
    cfa_data = c_low ? c_high : c_low;
     
    switch (cfa)
      {
      case DW_CFA_advance_loc:
      case DW_CFA_advance_loc1:
      case DW_CFA_advance_loc2:
      case DW_CFA_advance_loc4:
	switch (cfa)
	  {
	  case DW_CFA_advance_loc:
	    data = cfa_data;
	    break;
	  case DW_CFA_advance_loc1:
	    ci_read_1(data, frame);
	    break;
	  case DW_CFA_advance_loc2:
	    ci_read_2(data, frame);
	    break;
	  case DW_CFA_advance_loc4:
	    ci_read_4(data, frame);
	    break;
	  default:
	    data = 0;
	  }

	if (!init && cfa == DW_CFA_advance_loc)
	  edfmt_dwarf2_cfa_newstate(state);

	state->addr += data * header->code_align_factor;
	break;
      case DW_CFA_offset_extended:
      case DW_CFA_offset:
	 
	if (cfa == DW_CFA_offset)
	  reg = cfa_data;
	else
	  ci_uleb128(reg, frame);

	ci_leb128(data, frame);

	if (!CFA_C_REG(reg))
	  break;

	state->reg[reg].type = DW2_CFA_T_OFFSET;
	state->reg[reg].u.offset = data * header->data_align_factor;
	break;
      case DW_CFA_restore_extended:
      case DW_CFA_restore:
	if (init)
	  break;
	  
	if (cfa ==DW_CFA_restore)
	  data = cfa_data;
	else
	  ci_uleb128(data, frame);

	if (!CFA_C_REG(data))
	  break;
	  
	state->reg[data] = header->init.reg[data];
	break;
      case DW_CFA_set_loc:
	nloc = *(elfsh_Addr *) a_pos(frame);
	inc_pos(frame, sizeof(elfsh_Addr));
	if (nloc > state->addr)
	  {
	    if (!init)
	      edfmt_dwarf2_cfa_newstate(state);
	    state->addr = nloc;
	  }
	break;
      case DW_CFA_same_value:
      case DW_CFA_undefined:
	ci_uleb128(data, frame);

	if (!CFA_C_REG(data))
	  break;
	  
	memset(&state->reg[data], 0x00, sizeof(edfmtdw2cfareg_t));
	state->reg[data].type = (cfa == DW_CFA_undefined ? DW2_CFA_T_UNDEF : DW2_CFA_T_SAME);
	break;
      case DW_CFA_register:
	ci_uleb128(data, frame);
	ci_uleb128(reg, frame);

	if (!CFA_C_REG(reg) || !CFA_C_REG(data))
	  break;
	  
	state->reg[reg] = state->reg[data];
	break;
      case DW_CFA_remember_state:
	if (isave_pos >= 20)
	  break;
	save_stack[isave_pos++] = *state;
	break;
      case DW_CFA_restore_state:
	if (isave_pos <= 0)
	  break;
	*state = save_stack[--isave_pos]; 
	break;
      case DW_CFA_def_cfa:
	ci_uleb128(reg, frame);
	ci_uleb128(data, frame);

	state->cfa_reg = reg;
	state->cfa_offset = (elfsh_Addr) data;
	break;
      case DW_CFA_def_cfa_register:
	ci_uleb128(reg, frame);
	  
	state->cfa_reg = reg;
	break;
      case DW_CFA_def_cfa_offset:
	ci_uleb128(data, frame);
	  
	state->cfa_offset = (elfsh_Addr) data;
	break;
      case DW_CFA_nop:
	break;
      default:
	/* Unknown cfa, oupssss ? */
	break;
      }
  } while (dw2_sections.frame.pos < dw2_sections.frame.size &&
	   dw2_sections.frame.pos < max_pos);

  if (!init)
    {
      edfmt_dwarf2_cfa_newstate(state);
      state->addr = addr_start + addr_range;
    }

  /* If it's initialisation, we copy at the end */
  if (init)
    header->init = *state;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* .debug_frame parsing entry point */
int		edfmt_dwarf2_cfa()
{
  edfmtdw2cfahead_t header;
  edfmtdw2cfastate_t state;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Reset frame position */
  dw2_sections.frame.pos = 0;

  /* Reset state */
  memset(&state, 0x00, sizeof(edfmtdw2cfastate_t));

  do {
    /* Reset header */
    memset(&header, 0x00, sizeof(edfmtdw2cfahead_t));

    header.offset = dw2_sections.frame.pos;
    
    /* Read CIE header */
    ci_read_4(header.length, frame);
    header.end_offset = dw2_sections.frame.pos + header.length;
    ci_read_4(header.cid, frame);
    ci_read_1(header.version, frame);
    ci_str(header.augmentation, frame);
    ci_uleb128(header.code_align_factor, frame);
    ci_leb128(header.data_align_factor, frame);
    ci_read_1(header.return_addr_reg, frame);

    header.init_offset = dw2_sections.frame.pos;
    
    /* Read init */
    edfmt_dwarf2_cfa_data(&state, &header, 1);

    /* Correct length */
    if (header.end_offset != dw2_sections.frame.pos)
      dw2_sections.frame.pos = header.end_offset;

    do {
      edfmt_dwarf2_cfa_data(&state, &header, 0);

      /* We aren't at the end */
      if (dw2_sections.frame.pos < dw2_sections.frame.size)
	{
	  /* Check if the next header is a CIE */
	  if (header.cid == *(u_int *) (ac_pos(frame)+4))
	    break;
	}
    } while(dw2_sections.frame.pos < dw2_sections.frame.size);
  } while (dw2_sections.frame.pos < dw2_sections.frame.size);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
