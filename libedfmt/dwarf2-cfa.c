/*
** @file dwarf2-cfa.c
** @ingroup libedfmt
** Started Dec 26 2006 10:49:45 mxatone
**
**
** $Id: dwarf2-cfa.c,v 1.7 2007-03-07 16:45:35 thor Exp $
**
*/

#include "libedfmt.h"

/**
 * XXX: Re use it when it will be usefull
 * Submit a new row state 
 */
static int		edfmt_dwarf2_cfa_newstate(edfmtdw2cfastate_t *reg)
{
  edfmtdw2cfastate_t 	*tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (reg == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);  

  XALLOC(__FILE__, __FUNCTION__, __LINE__,tmp, sizeof(edfmtdw2cfastate_t), -1);
  memcpy(tmp, reg, sizeof(edfmtdw2cfastate_t));
  
  /* Add local and gloabal */
  /* TODO: Reuse when it will be usefull 
  if (current_cu->cfa == NULL)
    current_cu->cfa = tmp;
  else
    current_cu->last_cfa->next = tmp;

  current_cu->last_cfa = tmp;
  */
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}

/**
 * XXX: Re use it when it will be usefull
 * Parse .debug_frame FDE + data 
 */
static int		edfmt_dwarf2_cfa_data(edfmtdw2cfastate_t *state, 
					      edfmtdw2cfahead_t *header, unsigned char init)
{
  unsigned char		cfa, cfa_data, scfa, c_low, c_high;
  int			data, reg;
  eresi_Addr		nloc;
  edfmtdw2cfastate_t 	save_stack[20];
  int			isave_pos = 0;
  int      		cie;
  int      		length, addr_range, addr_start;
  unsigned long       		last_pos;
  unsigned long		max_pos;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

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
      dwarf2_iread_4(length, frame);	       	// Length (without length)
      last_pos = dwarf2_pos(frame);
      dwarf2_iread_4(cie, frame);		// Cie ptr
      dwarf2_iread_4(addr_start, frame); 	// Initial location
      dwarf2_iread_4(addr_range, frame);	// Address range
      
      state->addr = addr_start;      
    }
  else
    {
      last_pos = length = addr_range = addr_start = 0;
    }

  /* Max position (end of while loop) */
  max_pos = init ? header->end_offset : last_pos + length;
  
  do {
    dwarf2_iread_1(cfa, frame);
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
	    dwarf2_iread_1(data, frame);
	    break;
	  case DW_CFA_advance_loc2:
	    dwarf2_iread_2(data, frame);
	    break;
	  case DW_CFA_advance_loc4:
	    dwarf2_iread_4(data, frame);
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
	  dwarf2_iuleb128(reg, frame);

	dwarf2_ileb128(data, frame);

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
	  dwarf2_iuleb128(data, frame);

	if (!CFA_C_REG(data))
	  break;
	  
	state->reg[data] = header->init.reg[data];
	break;
      case DW_CFA_set_loc:
	nloc = *(eresi_Addr *) dwarf2_a_pos(frame);
	dwarf2_inc_pos(frame, sizeof(eresi_Addr));
	if (nloc > state->addr)
	  {
	    if (!init)
	      edfmt_dwarf2_cfa_newstate(state);
	    state->addr = nloc;
	  }
	break;
      case DW_CFA_same_value:
      case DW_CFA_undefined:
	dwarf2_iuleb128(data, frame);

	if (!CFA_C_REG(data))
	  break;
	  
	memset(&state->reg[data], 0x00, sizeof(edfmtdw2cfareg_t));
	state->reg[data].type = (cfa == DW_CFA_undefined ? DW2_CFA_T_UNDEF : DW2_CFA_T_SAME);
	break;
      case DW_CFA_register:
	dwarf2_iuleb128(data, frame);
	dwarf2_iuleb128(reg, frame);

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
	dwarf2_iuleb128(reg, frame);
	dwarf2_iuleb128(data, frame);

	state->cfa_reg = reg;
	state->cfa_offset = (eresi_Addr) data;
	break;
      case DW_CFA_def_cfa_register:
	dwarf2_iuleb128(reg, frame);
	  
	state->cfa_reg = reg;
	break;
      case DW_CFA_def_cfa_offset:
	dwarf2_iuleb128(data, frame);
	  
	state->cfa_offset = (eresi_Addr) data;
	break;
      case DW_CFA_nop:
	break;
      default:
	/* Unknown cfa, oupssss ? */
	break;
      }
  } while (dwarf2_pos(frame) < dwarf2_size(frame) &&
	   dwarf2_pos(frame) < max_pos);

  if (!init)
    {
      edfmt_dwarf2_cfa_newstate(state);
      state->addr = addr_start + addr_range;
    }

  /* If it's initialisation, we copy at the end */
  if (init)
    header->init = *state;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * XXX: Re use it when it will be usefull
 * .debug_frame parsing entry point 
 */
int			edfmt_dwarf2_cfa()
{
  edfmtdw2cfahead_t 	header;
  edfmtdw2cfastate_t 	state;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (dwarf2_data(frame) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      ".debug_frame section not available", -1);

  /* Reset frame position */
  dwarf2_pos(frame) = 0;

  /* Reset state */
  memset(&state, 0x00, sizeof(edfmtdw2cfastate_t));

  do {
    /* Reset header */
    memset(&header, 0x00, sizeof(edfmtdw2cfahead_t));

    header.offset = dwarf2_pos(frame);
    
    /* Read CIE header */
    dwarf2_iread_4(header.length, frame);
    header.end_offset = dwarf2_pos(frame) + header.length;
    dwarf2_iread_4(header.cid, frame);
    dwarf2_iread_1(header.version, frame);
    dwarf2_istr(header.augmentation, frame);
    dwarf2_iuleb128(header.code_align_factor, frame);
    dwarf2_ileb128(header.data_align_factor, frame);
    dwarf2_iread_1(header.return_addr_reg, frame);

    header.init_offset = dwarf2_pos(frame);
    
    /* Read init */
    edfmt_dwarf2_cfa_data(&state, &header, 1);

    /* Correct length */
    if (header.end_offset != dwarf2_pos(frame))
      dwarf2_pos(frame) = header.end_offset;

    do {
      edfmt_dwarf2_cfa_data(&state, &header, 0);

      /* We aren't at the end */
      if (dwarf2_pos(frame) < dwarf2_size(frame))
	{
	  /* Check if the next header is a CIE */
	  if (header.cid == *(unsigned int *) (dwarf2_ac_pos(frame)+4))
	    break;
	}
    } while(dwarf2_pos(frame) < dwarf2_size(frame));
  } while (dwarf2_pos(frame) < dwarf2_size(frame));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
