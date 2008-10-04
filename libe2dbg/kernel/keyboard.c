/**
 * @file libe2dbg/kernel/keyboard.c
 */
#include "ke2dbg.h"
#include "ke2dbg-keyb.h"

unsigned int ctrl_key=0;
unsigned int shift_key=0;
unsigned int alt_key =0;

unsigned char key_state;

int hook_rdtsc = 0;

unsigned int must_read = 1;
volatile unsigned char key_code_loc;
volatile unsigned int tmpo=0xdeadbeef;

char debug__[80];
char mouse_on;
char mouse;
char mouse1;
char mouse2;
char mouse3;
unsigned int led = 1;
unsigned int led_wait=0;
char led_tab[] = {2, 4, 1};

eresi_Addr	adresse_ret_1;
eresi_Addr	adresse_ret_2;

/* FIXME-XXX: originally this variable was negated at some point, but it disapeared
   See original rr0d code to understand what to do with that flag */
unsigned int edit_mode = 0;

/**
 * @return
 */
unsigned int ke2dbg_keyboard_wait(void)
{
  int i = 100;	// number of times to loop
  unsigned char mychar;
	
  do
    {
      mychar = ke2dbg_inb( KEYBOARD_PORT_64 );
      if(!(mychar & IBUFFER_FULL)) break;	// if the flag is clear, we go ahead
    }
  while (i--);

  if(i) return 1;
  return 0;
}

/**
 *  call ke2dbg_keyboard_wait before calling this function
 */
void ke2dbg_keyboard_read(void)
{
  int i = 100;	// number of times to loop
  unsigned char c;

  do
    {
      c = ke2dbg_inb(KEYBOARD_PORT_64);
      if(!(c & OBUFFER_FULL)) break;	// if the flag is clear, we go ahead	
      c = ke2dbg_inb(KEYBOARD_PORT_60);
    }
  while (i--);
}

/**
 * write a byte to the data port at 0x60
 */ 
unsigned int ke2dbg_keyboard_write(  unsigned char theCommand )
{
  if(1 == ke2dbg_keyboard_wait())
    {
      ke2dbg_keyboard_read();
      ke2dbg_outb( theCommand, KEYBOARD_PORT_60 );
    }
  else
    {
      return 0;
    }	
  // TODO: wait for ACK or RESEND from keyboard	
  return 1;
}

/**
 * @return
 */
unsigned char ke2dbg_keystroke_print(void)
{

  unsigned char state;
  unsigned char key_c;

  if (myke2dbg_keyboard_wait(&state))
    {
      key_state = state;
      if (!(key_state&0x20))
	{
	  //if (1)
	  key_c = ke2dbg_inb(KEYBOARD_PORT_60);

	  ke2dbg_handle_scancode(key_c);	  
	  
	  if (!fired)
	    {
	      myke2dbg_keyboard_wait(&state);
	      ke2dbg_outb(0xAD, KEYBOARD_PORT_64);
	      
	      myke2dbg_keyboard_wait(&state);
	      ke2dbg_outb(0xD2, KEYBOARD_PORT_64);
	      
	      myke2dbg_keyboard_wait(&state);
	      ke2dbg_outb(key_c, KEYBOARD_PORT_60);
	      
	      myke2dbg_keyboard_wait(&state);
	      ke2dbg_outb(0xAE, KEYBOARD_PORT_64);
	    }

	  return key_c;
	}
    }

  return 0;
}

/**
 * @param state
 */
unsigned int	myke2dbg_keyboard_wait(unsigned char* state)
{
  int		i = 0x100;	// number of times to loop
  unsigned char mychar;
	
  do
    {
      mychar = ke2dbg_inb(KEYBOARD_PORT_64);
      for (tmpo = 0; tmpo < 0x10; tmpo++) 
	tmpo = tmpo;
      
      // if the flag is clear, we go ahead
      if (mychar & 1) 
	break;	
    }
  while (i--);

  if (i >= 0) 
    {
      *state = mychar;
      return 1;
    }
  key_code_loc = 0;
  return 0;
}

/**
 * @return
 */
unsigned int ke2dbg_scancode_read(void)
{
  unsigned char state;
  unsigned char m1, m2, m3;
  unsigned char signe_X, signe_Y, of_X, of_Y, b1, b2, b3;
  int pX, pY;

  if (myke2dbg_keyboard_wait(&state))
    {
      key_state = state;
      if (!(key_state & 0x20))
	key_code_loc = ke2dbg_inb(KEYBOARD_PORT_60);          
      else
	{
	  myke2dbg_keyboard_wait(&state);
	  m1 = ke2dbg_inb(KEYBOARD_PORT_60);
	  myke2dbg_keyboard_wait(&state);
	  m2 = ke2dbg_inb(KEYBOARD_PORT_60);
	  myke2dbg_keyboard_wait(&state);
	  m3 = ke2dbg_inb(KEYBOARD_PORT_60);

	  b1 = m1 & 1;
	  b2 = m1 & 2;
	  b3 = m1 & 4;

	  signe_X = m1 & 16;
	  signe_Y = m1 & 32;

	  of_X = m1 & 64;
	  of_Y = m1 & 128;

	  m2 = signe_X?((~m2)):m2;
	  m3 = signe_Y?((~m3)):m3;

	  if (of_X)
	    m2 = 0;
	  if (of_Y)
	    m3 = 0;

	  pX = m2;
	  pY = m3;

	  if (pX>0)
	    pX=pX/3 +1;
	  if (pY>0)
	    pY=pY/3 +1;
	  
	  pX = signe_X?-pX:pX;
	  pY = signe_Y?pY:-pY;

	  ke2dbg_sprintf(debug__, "%d %d", pX, pY);
	  ke2dbg_logbuf_insert(debug__);
	  ke2dbg_cursor_update(cursor_X+pX, cursor_Y+pY);
	}
    }

  return key_code_loc;
}




/** 
 * Translate raw keyboard data to ascii 
 * @param scancode
 * @return
 */
char ke2dbg_scan_to_ascii(int scancode)
{
  int down = scancode & 0x80;

  switch (scancode & 0x7f)
    {
    case 42:
    case 54:
      shift_key = !down;
      break;

    case 29:
      ctrl_key = !down;
      break;

    case 56:
      alt_key = !down;
      break;
      /*
	case 58:
	shift_key = !shift_key;
	break;
      */
    }

  switch (scancode)
    {
    case 14: return KBDEL;
    case 88: return KBF12;
    case 87: return KBF11;
    case 72: return KBCURSORUP;
    case 80: return KBCURSORDOWN;
    case 56: return KBLEFTALT;
    case 54: return KBRIGHTSHIFT;
    case 42: return KBLEFTSHIFT;
    case 73: return KBPAGEUP;
    case 81: return KBPAGEDOWN;
    case 75: return KBCURSORLEFT;
    case 77: return KBCURSORRIGHT;
    case 1: return KBESC;

    }; 

  if (scancode <= 0x80)
    return (shift_key ? scantable_MAJ[scancode] : 
	    scantable_N_MAJ[scancode]);    
	
  return KBUP;
}




/**
 * essaye de remonter le listing asm
 * de facon un peu pourie (brute    
 * force...)                        
 *
 * @param cur_eip
 * @param min_back_line
 * @return
 */
long		back_disasm(unsigned int cur_eip, int min_back_line)
{
  int		back_dec = (min_back_line<10)?10*3:min_back_line*3;
  unsigned int	tmp_eip;
  int		num_instr=0;
  int		ret;
  int	       guard=15;
  unsigned int testing_eip;
  asm_instr	my_ins;

  cur_eip=ke2dbg_virt2lin_convert(current_reg_stack.reg_CS, cur_eip  );

  if (!(ke2dbg_page_exist(cur_eip)))
    return 0;

  while((!ke2dbg_page_exist(cur_eip-back_dec))&& (back_dec>0))
    back_dec--;

  if (back_dec==0)
    return 0;

  testing_eip= cur_eip - back_dec;

  do
    {
      if (!ke2dbg_page_exist(testing_eip-1))
	break;

      testing_eip--;
      tmp_eip = testing_eip;
      num_instr=0;

      do
	{
	  ret = asm_read_instr(&my_ins, (unsigned char *) tmp_eip, 32, world.curjob->proc);
	  tmp_eip += ret;
	  num_instr++;  
	} while ((ret) && (tmp_eip < cur_eip));
      
      guard--;

    } while ((tmp_eip != cur_eip) && (guard >= 0));

  while (num_instr > min_back_line)
    {
      testing_eip += asm_read_instr(&my_ins, (unsigned char *) testing_eip, 32, world.curjob->proc);
      num_instr--;
    }

  return (cur_eip - testing_eip);
}


/** 
 * ajoute un caractere a la commande courante 
 *
 * @param car
 */
void ke2dbg_cmdkey_insert(char car) /* A new key is pressed and is added to the command */
{
  accessline[cmd_win_y]=1;
  Command[CommandLength]=car;

  screen_lines[cmd_win_y][1+CommandLength]=car;
  CommandLength++;
  Command[CommandLength]=0x0;
}

/**
 * Clear current command line 
 */
void ke2dbg_command_clear()                
{
  ke2dbg_line_delete(cmd_win_y);   
  CommandLength=0;
}

/**
 * Remove char from current line (= backspace) 
 */
void ke2dbg_cmdkey_delete()
{
  accessline[cmd_win_y]=1;
  if (CommandLength) {

    screen_lines[cmd_win_y][CommandLength]=' ';

		 
    CommandLength--;
    Command[CommandLength]=0x0;
  };      
}


/** 
 * Handler for ENTER key 
 */
void		ke2dbg_keyb_enter()
{
  char		**argv;
  int		argc;

  if (!fired || !CommandLength)
    return;  
  ke2dbg_logbuf_insert(Command);
  ke2dbg_command_insert(Command);

  /* Parse command for the kernel debugger */
  argv = revm_input(&argc, Command);
  if (argv == ((char **) REVM_INPUT_VOID))
    return;
  else if (!argv)
    {
      revm_cleanup();
      return;
    }
  if (revm_parseopt(argc, argv) < 0)
    {
      //XFREE(__FILE__, __FUNCTION__, __LINE__, argv);
      return;
    }

  /* FIXME-XXX: Seems like this is necessary at some 
     point but not sure yet in which commands, only 
     when the original parse_command() return those
     values.
     
     switch (ret)
     {
     case 2:
      data_ptr = adresse_ret_1;
      break;
      
      case 3:
      code_ptr = adresse_ret_2;
      force_disasm = 1;
      update_disasm = 1;
      }
  */

  world.curjob->curcmd = world.curjob->script[0];
  switch (revm_execmd())
    {
    case REVM_SCRIPT_CONTINUE:
      //printf(" [*] e2dbg continue from revm_execmd \n");
      revm_cleanup();
      return;
    case REVM_SCRIPT_ERROR:
      profiler_error();
    default:
      break;
    }

  ke2dbg_command_clear();
  ke2dbg_line_delete(cmd_win_y);
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
}



/*
int		old_keyb_enter()
{
  int		ret;

  ret =  parse_command(Command);
  if (ret > 1)
    {
      ke2dbg_logbuf_insert(cmd_result);
      
      switch (ret)
	{
	case 2:
	  data_ptr = adresse_ret_1;
	  break;
	  
	case 3:
	  code_ptr = adresse_ret_2;
	  force_disasm = 1;
	  update_disasm = 1;
	  break;
	  
	case 4:
	  cmd_rr0d_rasta();
	  break;
	  
	case 5:
	  cmd_rr0d_rasta_off();
	  break;
	  
	case 6:
	  cmd_rr0d_set();
	  break;
	  
	case 8:
	  cmd_help();
	  break;
	  
	case 9:
	  cmd_rr0d_insertbp();
	  break;
	  
	case 10:
	  cmd_rr0d_removebp();
	  break;
	  
	case 11:
	  ke2dbg_display_breakpoints();
	  break;
	  
	case 12:
	  cmd_rr0d_idt();
	  break;
	  
	case 13:
	  cmd_rr0d_bpm();
	  break;	  

	case 15:
	  cmd_rr0d_info();
	  break;

	case 16:
	  cmd_sym();
	  break;

	case 17:
	  cmd_rr0d_editmem();
	  break;

	case 18:
	  cmd_rr0d_inthook();
	  break;

	case 19:
	  cmd_rr0d_rdtschook();
	  break;

	case 20:
	  cmd_rr0d_foolmode();
	  break;

	case 21:
	  cmd_rr0d_unfoolmode();
	  break;

	case 0xff:
	  cmd_rr0d_print();
	  break;

	default:	  
	  ke2dbg_sprintf(debug__, "%.8X", adresse_ret_1);
	  ke2dbg_logbuf_insert(debug__);
	  break;
	}      
    }
  else
    ke2dbg_logbuf_insert("BaD Command man");
  
  ke2dbg_command_clear();
  ke2dbg_line_delete(cmd_win_y);
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
}
*/

/**
 * @param scancode
 */
void		ke2dbg_handle_scancode(unsigned char scancode)
{
  int		i;
  char		car;
  int		len;
  unsigned int local_data_ptr;
  unsigned int local_data_byte;

  if ((scancode & 0x7f ) == CTRL_KEY)
    {
      if (scancode & 0x80)
	ctrl_key = 0;
      else
	ctrl_key = 1;
    }

  if (ctrl_key)
    if ((scancode & 0x7f ) == F_KEY)
      {
	if (!(scancode & 0x80))
	  {

	    ctrl_key = 0;
	    fired = !fired;
	    return;
	  }
      }

  car = ke2dbg_scan_to_ascii((int) scancode);
  switch (car)
    {
      
      /* Exec a commandSi se pulsó enter borra linea y indice igual a 0 */
    case KBENTER:              
      ke2dbg_keyb_enter();
      break;

      /* Delete the last character pressed */
    case KBDEL: 
      if (fired) 
	ke2dbg_cmdkey_delete();  
      break;
     
    case KBPAGEUP:

      /*  
	  if ((infobuffer==ACTIVADO) && (fired))
	  ke2dbg_logbuf_upline();
	  else if ((disasmindex!=0) && (fired))
	  {retrocededesasm();
	  };
      */
      
      if (fired)
	{
	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X, cursor_Y-10);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      ke2dbg_logbuf_uplines();
	      ke2dbg_logbuf_print();
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr-=0x100;

	    }


	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      /*
		len=0;
		for (i=0;i<code_win_dim;i++)
		{
		len += code_dim_line[i];

		}
		code_ptr-=len?len:0x20;*/
	      len= back_disasm (code_ptr, code_win_dim);
	      code_ptr-=len?len:0x20;
	      force_disasm = 1;
	      update_disasm = 1;


	    }

	}
      break;

    case KBPAGEDOWN:		 
      if (fired)
	{
	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X, cursor_Y+10);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      ke2dbg_logbuf_downlines();
	      ke2dbg_logbuf_print();
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr+=0x100;

	    }

	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      len=0;
	      for (i=0;i<code_win_dim;i++)
		{
		  len += code_dim_line[i];

		}
	      code_ptr+=len?len:0x20;
	      force_disasm = 1;
	      update_disasm = 1;
	    }

	}

      break;
                    
    case KBF12:
      break;

      /* Move up in the hexadecimal display windows */
    case KBCURSORUP:             

      if (fired)
	{
	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X, cursor_Y-1);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      ke2dbg_logbuf_upline();
	      ke2dbg_logbuf_print();
	      break;
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr-=0x10;
	      break;

	    }


	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      /*
		code_ptr-=code_dim_line[0];
	      */
	      len= back_disasm (code_ptr, 1);
	      code_ptr-=len?len:3;
	      force_disasm = 1;
	      update_disasm = 1;
	      break;
					
	    }


	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_y--;
	      ke2dbg_console_on();
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(shift_key))
	    {
	      ke2dbg_command_recall(-1);
	      ke2dbg_display_refresh();
	      break;
	    }



	}
      break;
 
      /* Move down in the hexadecimal display windows */
    case KBCURSORDOWN:           

      if (fired)
	{

	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X, cursor_Y+1);
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(!shift_key))
	    {
	      ke2dbg_logbuf_downline();
	      ke2dbg_logbuf_print();
	      break;
	    }

	  if (((alt_key)&&(!ctrl_key)&&(!shift_key)))
	    {
	      data_ptr+=0x10;
	      break;

	    }

	  if ((!alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      code_ptr+=code_dim_line[0];
	      force_disasm = 1;
	      update_disasm = 1;
	      break;
	    }


	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_y++;
	      ke2dbg_console_on();
	      break;
	    }

	  if ((!alt_key)&&(!ctrl_key)&&(shift_key))
	    {
	      ke2dbg_command_recall(1);
	      ke2dbg_display_refresh();
	      break;
	    }


	}
      break;

    case KBCURSORLEFT:
		      
      if (fired)
	{

	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X-1, cursor_Y);
	      break;
	    }

	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_x--;
	      ke2dbg_console_on();
	      break;
	    }

	}
      break;

    case KBCURSORRIGHT:
		      
      if (fired)
	{

	  if (edit_mode)
	    {
	      ke2dbg_cursor_update(cursor_X+1, cursor_Y);
	      break;
	    }

	  if ((alt_key)&&(ctrl_key)&&(!shift_key))
	    {
	      screen_offset_x++;
	      ke2dbg_console_on();
	      break;
	    }

	}
      break;


    case KBESC:
      edit_mode = 0;
      ke2dbg_cursor_erase();
      break;

      /* Activate or desactivate trap mode */
    case KBLEFTALT:             
      break;

    case KBRIGHTSHIFT:

    case KBLEFTSHIFT:
      break;

    case KBUP:
      break;

    case 0:
      break;

    default:                 
      if (!fired)
	break;

      if (!edit_mode)
	{
	  ke2dbg_cmdkey_insert(car);
	  break;
	}

      if (!((car>='0' &&  car <='9')||
	    (car>='a' &&  car <='f')||
	    (car>='A' &&  car <='F')))
	break;
      
      if ((car>='0' &&  car <='9'))
	car-='0';
	      
      if ((car>='a' &&  car <='f'))
	car=car-'a'+10;
	      
      if ((car>='A' &&  car <='F'))
	car=car-'A'+10;
	      	      
      local_data_ptr = data_ptr + 
	(cursor_Y-data_win_y) * 0x10 + (cursor_X-14)/3;

      if (ke2dbg_page_exist(local_data_ptr))
	{
	  local_data_byte = *((unsigned char*)local_data_ptr);
	  switch((cursor_X)%3)
	    {
	    case 0:
	      local_data_byte&=0xf;
	      local_data_byte+=car<<4;
	      break;
		      
	    case 1:
	      local_data_byte&=0xf0;
	      local_data_byte+=car;
	      break;
	    }
	  write_save_dirty( (unsigned char*)local_data_ptr, 
			    (unsigned char)local_data_byte);
	}
	      
      ke2dbg_cursor_update(cursor_X+1, cursor_Y);
      ke2dbg_data_dump(current_reg_stack.reg_DS, data_ptr);
    }
           
}
