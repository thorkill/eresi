/**
 * @file libe2dbg/kernel/commands.c
 *
 *
 */
#include "ke2dbg.h"


/**
 *
 *
 */
void		ke2dbg_commands_install()
{
  revm_command_add(CMD_RASTA , (void *) cmd_rr0d_rasta, (void *) NULL, 0, "");
  revm_command_add(CMD_RASTA_OFF, (void *) cmd_rr0d_rasta_off, (void *) NULL, 0, "");
  revm_command_add(CMD_FOOL, (void *) cmd_rr0d_foolmode, (void *) NULL, 0, "");
  revm_command_add(CMD_FOOL_OFF, (void *) cmd_rr0d_unfoolmode, (void *) NULL, 0, "");
  revm_command_add(CMD_IDT , (void *) cmd_rr0d_idt, (void *) NULL, 0, "");
  revm_command_add(CMD_BPM , (void *) cmd_rr0d_bpm, (void *) revm_getoption, 1, "");
  revm_command_add(CMD_RINFO, (void *) cmd_rr0d_info, (void *) revm_getoption, 1, "");
  revm_command_add(CMD_EDITMEM, (void *) cmd_rr0d_editmem, (void *) NULL, 0, "");
  revm_command_add(CMD_INTHOOK, (void *) cmd_rr0d_inthook, (void *) revm_getoption, 0, "");
  revm_command_add(CMD_RDTSCHOOK, (void *) cmd_rr0d_rdtschook, (void *) NULL, 0, "");

  /* It is lacking some commands ! */
  /*
    cmd_rr0d_set
    cmd_rr0d_print
    cmd_rr0d_insertbp
    cmd_rr0d_removebp

    what about stepping ?

    @todo need to add b/delete/stepping to eresi vectors
  */

}

/**
 *
 * @return Always 0
 */
int		cmd_rr0d_rasta()
{
  rasta_mode = 1;
  ke2dbg_allwin_refresh();
  return (0);
}

/**
 *
 * @return Always 0
 */
int		cmd_rr0d_rasta_off()
{
  rasta_mode = 0;
  ke2dbg_allwin_refresh();
  return (0);
}

/**
 *
 * @return Always 0
 */
int		cmd_rr0d_idt()
{
  ke2dbg_idt_print((unsigned long *) idt_[0]);
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_bpm()
{
  ke2dbg_hwbp_set(ke2dbg_virt2lin_convert(current_reg_stack.reg_CS, adresse_ret_1));
  return (0);
}



/**
 *
 * @return Always 0
 */
int		cmd_rr0d_info()
{
  ke2dbg_page_display(adresse_ret_1);
  kedbg_physpage_get(adresse_ret_1);
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_editmem()
{
  if (cursor_X == 0 && cursor_Y == 0)
    {
      cursor_X = 15;
      cursor_Y = data_win_y;
    }
  
  cursor_old_color = screen_colors[cursor_Y][cursor_X];
  ke2dbg_cursor_update(cursor_X, cursor_Y);
  ke2dbg_logbuf_insert("MeM Edit mode");
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_inthook()
{
  ke2dbg_sprintf(debug__, "inthook N=%d ", adresse_ret_1);
  ke2dbg_logbuf_insert(debug__);
  if (adresse_ret_1 >=MAX_INT)
    {
      ke2dbg_logbuf_insert("too big int, man");
      return (-1);
    }
  
  if (is_int_hooked[adresse_ret_1])
    tmp_int[adresse_ret_1] =(void *) ke2dbg_idt_modify( (unsigned int)old_int[adresse_ret_1],
						  (unsigned int*)idt_[0]+2*adresse_ret_1);
  else
    {
      if (addrnewint[adresse_ret_1]==(unsigned long)NULL)
	{
	  ke2dbg_sprintf(debug__, 
			 "humm, ke2dbg doesn't know how to handle int %d yet, man", adresse_ret_1);
	  ke2dbg_logbuf_insert(debug__);
	  return (-1);
	}
      old_int[adresse_ret_1] =(void *) ke2dbg_idt_modify( (unsigned int)addrnewint[adresse_ret_1],
						    (unsigned int*)idt_[0]+2*adresse_ret_1); 
    }
  is_int_hooked[adresse_ret_1] = is_int_hooked[adresse_ret_1] ? 0 : 1;
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_rdtschook()
{
  /* TEST RDTSC disable */
  if (hook_rdtsc)
    ke2dbg_cr4_set(ke2dbg_cr4_get() & (~0x4));
  else
    ke2dbg_cr4_set(ke2dbg_cr4_get() | (0x4));
  hook_rdtsc = (hook_rdtsc ? 0 : 1);
  ke2dbg_sprintf(debug__, "AP cr4: %.8X",ke2dbg_cr4_get()); 
  Out_Debug_String(debug__);
  ke2dbg_output(debug__);
  return (0);
}

/**
 *
 * @return Always 0
 */
int		cmd_rr0d_foolmode()
{
  fOOl_mode = 1;
  ke2dbg_allwin_refresh();
  return (0);
}

/**
 *
 * @return Always 0
 */
int		cmd_rr0d_unfoolmode()
{
  fOOl_mode = 0;
  ke2dbg_allwin_refresh();
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_print()
{
  eresi_Addr	val;

  switch(adresse_ret_2)
    {
    case 0:
      val = reg_stack->reg_EAX;
      break;
    case 1:
      val = reg_stack->reg_EBX;
      break;
    case 2:
      val = reg_stack->reg_ECX;
      break;
    case 3:
      val = reg_stack->reg_EDX;
      break;
    case 4:
      val = reg_stack->reg_ESI;
      break;
    case 5:
      val = reg_stack->reg_EDI;
      break;
    case 6:
      val = reg_stack->reg_EIP;
      break;
    case 7:
      val = reg_stack->reg_ESP;
      break;
    case 8:
      val = reg_stack->reg_EBP;
      break;
    case 9:
      val = ke2dbg_dr0_get();
      break;
    case 10:
      val = ke2dbg_dr1_get();
      break;
    case 11:
      val = ke2dbg_dr2_get();
      break;
    case 12:
      val = ke2dbg_dr3_get();
      break;
    case 13:
      val = ke2dbg_dr6_get();
      break;
    case 14:
      val = ke2dbg_dr7_get();
      break;
    default:
      val = 0xDEADFACE;
      break;
    }
  ke2dbg_sprintf(debug__, "%.8X", val);
  ke2dbg_logbuf_insert(debug__);
  return (0);
}




/**
 *
 * @return Always 0
 */
int		cmd_rr0d_set()
{
  switch (adresse_ret_2)
    {
    case 0:
      reg_stack->reg_EAX = adresse_ret_1;
      break;
    case 1:
      reg_stack->reg_EBX = adresse_ret_1;
      break;
    case 2:
      reg_stack->reg_ECX = adresse_ret_1;
      break;
    case 3:
      reg_stack->reg_EDX = adresse_ret_1;
      break;
    case 4:
      reg_stack->reg_ESI = adresse_ret_1;
      break;
    case 5:
      reg_stack->reg_EDI = adresse_ret_1;
      break;
    case 6:
      reg_stack->reg_EIP = adresse_ret_1;
      break;
    case 7:
      reg_stack->reg_ESP = adresse_ret_1;
      break;
    case 8:
      reg_stack->reg_EBP = adresse_ret_1;
      break;
    default:
      break;
    }
  ke2dbg_sprintf(debug__, "%X <= cr3", ke2dbg_basetp());
  ke2dbg_logbuf_insert(debug__);
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_insertbp()
{
  ke2dbg_insert_bp(current_reg_stack.reg_CS, adresse_ret_1, BP_CLASSIC);
  return (0);
}


/**
 *
 * @return Always 0
 */
int		cmd_rr0d_removebp()
{
  if (ke2dbg_erase_breakpoint(adresse_ret_1)==-1)
    {
      ke2dbg_logbuf_insert("not a green break point, man");
      return (-1);
    }
  return (0);
}
