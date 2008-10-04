/**
 * @file libe2dbg/kernel/interrupts.c
 *
 */
#include "ke2dbg.h"


/* FB PCI AGP */
unsigned char fb_hw_padding_var1[0x1000];
unsigned char fb_hw_flying_page1[1024*0x1000];
unsigned char fb_hw_padding_var2[0x1000];
unsigned int fb_hw_flying_page1_sav = 0;
unsigned int fb_hw_flying_page1_level = 0;
/********************/

unsigned int	stackptr;
void		*oldkbd;
void		*old_int[MAX_INT];
void		*tmpkbd;
void		*tmp_int[MAX_INT];
unsigned long   addrnewkbd;
unsigned long	addrnewint[MAX_INT];
int		is_int_hooked[MAX_INT];

char		debug_[1024] = {0};
unsigned char	key_scan_code = 0;
int		fired = 0;
unsigned int	in_hlt = 0;
Reg_struct	old_reg_data = {0};
unsigned int	asm_lines_adresse[100]={0};
unsigned int	asm_lines_adresse_n = 0;
char		asm_line[78] = "                                                                              ";
char		tab_true_byte[500];

int		num_old_bp = -1;
int		step_on = 0;
int		spawn_bp = 0;
int		spawn_hw_bp = 0;
int		id_hw_bp = 0;
unsigned int	dr7_value = 0;
unsigned int	old_id_hw_bp = 0;

int bp_num;
int test_=0;

unsigned int uuu;
unsigned int gdt_limit;
unsigned long idt_[2];
unsigned long gdt_[2];
unsigned long ldt_[1];
eresi_Addr	data_ptr;
unsigned int old_data_ptr;
unsigned int update_disasm=1;
unsigned int force_disasm=0;
eresi_Addr	code_ptr;
eresi_Addr	old_code_ptr;
Reg_struct current_reg_stack;
Reg_struct *reg_stack;
eresi_Addr	stack_ptr;
unsigned int oo;
unsigned int saved_eflag;
eresi_Addr current_CS;
eresi_Addr current_DS;
eresi_Addr ring0_ES;
eresi_Addr ring0_FS;
eresi_Addr ring0_GS;
unsigned int mycr2;

volatile unsigned int ooooooo;

#define STEP_INTO	0x42
#define STEP_OVER	0x44
#define RUN		0xd8

unsigned char scratch[0x100]="\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
????????????????\
";





/** 
 * Print register state when kernel stopped	  
 * @bug affichage des GS FS en 16bit              
 * @param regstack
*/
void ke2dbg_register_print(Reg_struct *regstack)
{

  ke2dbg_sprintf( debug_, "%.8X", regstack->reg_EAX);
  ke2dbg_text_display(5 , 1, debug_);
  if (regstack->reg_EAX== old_reg_data.reg_EAX)
    ke2dbg_memcpy(&screen_colors[1][5], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[1][5], "11111111", 8);
  
  ke2dbg_sprintf( debug_, "%.8X", regstack->reg_EBX);
  ke2dbg_text_display(18, 1, debug_);
  if (regstack->reg_EBX== old_reg_data.reg_EBX)
    ke2dbg_memcpy(&screen_colors[1][18], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[1][18], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_ECX );
  ke2dbg_text_display(31, 1, debug_);
  if (regstack->reg_ECX== old_reg_data.reg_ECX)
    ke2dbg_memcpy(&screen_colors[1][31], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[1][31], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_EDX );
  ke2dbg_text_display(44, 1, debug_);
  if (regstack->reg_EDX== old_reg_data.reg_EDX)
    ke2dbg_memcpy(&screen_colors[1][44], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[1][44], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_EDI );
  ke2dbg_text_display(57, 1, debug_);
  if (regstack->reg_EDI== old_reg_data.reg_EDI)
    ke2dbg_memcpy(&screen_colors[1][57], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[1][57], "11111111", 8);


  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_ESI );
  ke2dbg_text_display(5 , 2, debug_);
  if (regstack->reg_ESI== old_reg_data.reg_ESI)
    ke2dbg_memcpy(&screen_colors[2][5], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[2][5], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_EBP );
  ke2dbg_text_display(18, 2, debug_);
  if (regstack->reg_EBP== old_reg_data.reg_EBP)
    ke2dbg_memcpy(&screen_colors[2][18], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[2][18], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_ESP );
  ke2dbg_text_display(31, 2, debug_);
  if (regstack->reg_ESP== old_reg_data.reg_ESP)
    ke2dbg_memcpy(&screen_colors[2][31], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[2][31], "11111111", 8);

  ke2dbg_sprintf( debug_, "%.8X",regstack->reg_EIP );
  ke2dbg_text_display(44, 2, debug_);
  if (regstack->reg_EIP== old_reg_data.reg_EIP)
    ke2dbg_memcpy(&screen_colors[2][44], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[2][44], "11111111", 8);


  debug_[4]=0;
  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_CS);
  ke2dbg_text_display(5 , 3, debug_);
  if (regstack->reg_CS== old_reg_data.reg_CS)
    ke2dbg_memcpy(&screen_colors[3][5], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][5], "11111111", 8);

  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_DS);
  ke2dbg_text_display(18, 3, debug_);
  if (regstack->reg_DS== old_reg_data.reg_DS)
    ke2dbg_memcpy(&screen_colors[3][18], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][18], "11111111", 8);

  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_ES);
  ke2dbg_text_display(31, 3, debug_);
  if (regstack->reg_ES== old_reg_data.reg_ES)
    ke2dbg_memcpy(&screen_colors[3][31], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][31], "11111111", 8);

  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_SS);
  ke2dbg_text_display(44, 3, debug_);
  if (regstack->reg_SS== old_reg_data.reg_SS)
    ke2dbg_memcpy(&screen_colors[3][44], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][44], "11111111", 8);

  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_FS);
  ke2dbg_text_display(57, 3, debug_);
  if (regstack->reg_FS== old_reg_data.reg_FS)
    ke2dbg_memcpy(&screen_colors[3][57], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][57], "11111111", 8);

  ke2dbg_sprintf(debug_, "%.2X", regstack->reg_GS);
  ke2dbg_text_display(70, 3, debug_);
  if (regstack->reg_GS== old_reg_data.reg_GS)
    ke2dbg_memcpy(&screen_colors[3][70], "        ", 8);
  else
    ke2dbg_memcpy(&screen_colors[3][70], "11111111", 8);


  ke2dbg_sprintf(debug_, "%.8X", key_scan_code);
  ke2dbg_text_display(43, cmd_win_y+1, debug_);


  ke2dbg_text_display(53, 2, "FLG=");


  ke2dbg_sprintf( debug_, "%c%c%c%c%c%c%c%c%c",
	   regstack->reg_EFLAG&0x800?'O':'o',
	   regstack->reg_EFLAG&0x400?'D':'d',
	   regstack->reg_EFLAG&0x200?'I':'i',
	   regstack->reg_EFLAG&0x100?'T':'t',
	   regstack->reg_EFLAG&0x80 ?'S':'s',
	   regstack->reg_EFLAG&0x40 ?'Z':'z',
	   //20
	   regstack->reg_EFLAG&0x10 ?'A':'a',
	   //8
	   regstack->reg_EFLAG&0x4 ?'P':'p',
	   //2
	   regstack->reg_EFLAG&0x1 ?'C':'c' );
  ke2dbg_text_display(57, 2, debug_);


  screen_colors[2][57]= (regstack->reg_EFLAG&0x800)==(old_reg_data.reg_EFLAG&0x800)?' ':'1';
  screen_colors[2][58]= (regstack->reg_EFLAG&0x400)==(old_reg_data.reg_EFLAG&0x400)?' ':'1';
  screen_colors[2][59]= (regstack->reg_EFLAG&0x200)==(old_reg_data.reg_EFLAG&0x200)?' ':'1';
  screen_colors[2][60]= (regstack->reg_EFLAG&0x100)==(old_reg_data.reg_EFLAG&0x100)?' ':'1';
  screen_colors[2][61]= (regstack->reg_EFLAG&0x80) ==(old_reg_data.reg_EFLAG&0x80)?' ':'1';
  screen_colors[2][62]= (regstack->reg_EFLAG&0x40) ==(old_reg_data.reg_EFLAG&0x40)?' ':'1';
  screen_colors[2][63]= (regstack->reg_EFLAG&0x10) ==(old_reg_data.reg_EFLAG&0x10)?' ':'1';
  screen_colors[2][64]= (regstack->reg_EFLAG&0x4)  ==(old_reg_data.reg_EFLAG&0x4)?' ':'1';
  screen_colors[2][65]= (regstack->reg_EFLAG&0x1)  ==(old_reg_data.reg_EFLAG&0x1)?' ':'1';

  if (fired)
    ke2dbg_sprintf(debug_, "%.8X", 0xDEADBEEF);
  else
    ke2dbg_sprintf(debug_, "%.8X", 0);

  ke2dbg_text_display(43, cmd_win_y+2, debug_);

  ke2dbg_display_refresh();
}



/**
 * affiche les codes des touches appuyées          
 * @param oo
 *
 */
void ke2dbg_keystate_print (unsigned int oo)
{

  
  ke2dbg_sprintf(debug_, "Key: %.8X", oo);
  ke2dbg_text_display(52, cmd_win_y+1, debug_);

  ke2dbg_sprintf(debug_, "shift: %.1X ctrl: %.1X alt: %.1X", 
	  shift_key, 
	  ctrl_key,
	  alt_key);
  ke2dbg_text_display(52, cmd_win_y, debug_);
  ke2dbg_display_refresh();
  
}




/**
 * affiche le contenu de la fenetre de code        
 * disasm le code a l'adresse specifie             
 * si la page memoire de l'adresse specifiee       
 * pas, affiche le disasm de ?????                 
 * cache egalement les octet des brea points       
 * BUG: lors de la remontee du code asm, mauvaise  
 * adresse. (retro dasm...)                        
 * 
 * @param selector
 * @param current_eip                                               
 */
void ke2dbg_printf_disasm(eresi_Addr selector, eresi_Addr current_eip)
{
  unsigned int i, ii;
  unsigned int vic;
  unsigned int decal_printf=0;
  unsigned int x=0;
  unsigned int index_chaine=0;

  eresi_Addr	virtual_eip;

  unsigned char *symbol_name;
  unsigned int	j;
  char		asm_out[80];
  asm_instr	my_ins;

  if (current_eip != 0)
    code_ptr = (unsigned int)current_eip;

  selector = selector & 0xFFFF;
  virtual_eip = ke2dbg_virt2lin_convert(selector, (unsigned int)current_eip);
  current_eip = virtual_eip;

  if ( (!ke2dbg_page_exist(virtual_eip)) || (!ke2dbg_page_exist(virtual_eip+0x70)) )
    {
      for (i=0;i<0x70;i++)
	tab_true_byte[i] = '?';
    }
  else
    {
      for (i=0;i<0x70;i++)
	tab_true_byte[i] = ke2dbg_true_byte_bp((char *) current_eip + i);
    }



  asm_lines_adresse_n = 0;		
  for (i=0;i+decal_printf<code_win_dim;i++,asm_lines_adresse_n++)
    {
      index_chaine=0;

      if (ke2dbg_is_breakpoint((char *) current_eip + x) != -1 || 
	  ke2dbg_is_hw_breakpoint((char *) current_eip + x))
	{
	  for (ii=14;ii<78;ii++)
	    screen_colors[code_win_y+i][ii]='1';
	}
      else if ((current_eip + x) == 
	       ke2dbg_virt2lin_convert(selector, (unsigned int)reg_stack->reg_EIP))
	{
	  for (ii=14;ii<78;ii++)
	    screen_colors[code_win_y+i][ii]='2';
	}
      else
	{
	  for (ii=14;ii<78;ii++)
	    screen_colors[code_win_y+i][ii]=' ';
	}
      
      symbol_name = elfsh_reverse_symbol(world.curjob->curfile, current_eip + x, NULL);
      
      if (symbol_name)
	{

	  for (j=1;j<15;j++)
	    {
	      put_XY_char(j, code_win_y+i+decal_printf, ' ');
	    }

	  j=0;
	  while(symbol_name[j])
	    {
	      put_XY_char(j+15, code_win_y+i+decal_printf, symbol_name[j]);
	      j++;
	    }

	  put_XY_char(j+15, code_win_y+i+decal_printf, ':');
		   
	  for (j++;j<78-15;j++)
	    {
	      put_XY_char(j+15, code_win_y+i+decal_printf, ' ');

	    }

	  decal_printf++;
	  //		    printf("symbol found:) %s", symbol_name);
	}
      //#endif

      if (!(i+decal_printf<code_win_dim))
	break;

      /*end symbol*/
      index_chaine += ke2dbg_sprintf(asm_line, "%.4X:%.8X ", selector, current_eip + x);

      asm_lines_adresse[asm_lines_adresse_n++] = current_eip + x;
      
      // FIXME: instruction ascii should be pointed by asm_out !
      //vic=disasm(&tab_true_byte[x], asm_out,32,(long)&current_eip[x],0, &my_ins);
      
      vic = asm_read_instr(&my_ins, (char *) tab_true_byte + x + current_eip + x,
			   32, world.curjob->proc);
      if (!vic)
	{
	  ke2dbg_sprintf(asm_out, "0x%.2X", *(unsigned char *) current_eip + x);
	  vic=1;
	}

      code_dim_line[i] = vic;

      for (ii = 0 ; ii<vic;ii++)
	index_chaine+=ke2dbg_sprintf(asm_line+index_chaine, "%.2X", tab_true_byte[x+ii]&0xff);

      for (;ii<10;ii++)
	{
	  asm_line[index_chaine++] = ' ';
	  asm_line[index_chaine++] = ' ';
	}

      index_chaine+=ke2dbg_sprintf(asm_line+index_chaine, "%s", asm_out);

      for (ii=index_chaine;ii<78;ii++)
	asm_line[index_chaine++] = ' ';

      if ((current_eip + x) == ke2dbg_virt2lin_convert(selector, (unsigned int)reg_stack->reg_EIP))
	asm_line[77] ='<'; 
      ke2dbg_print_CW(asm_line, i+decal_printf);

      x+=vic;
    }

  ke2dbg_codewin_refresh();
  ke2dbg_display_refresh();

}



/**
 * affiche la fenetre data. si les pages memoire   
 * de l'adresse ne sont pas presente, affiche des  
 * ?????
 * @param selecter
 * @param ptr
 */
void ke2dbg_data_dump(eresi_Addr selector, eresi_Addr ptr)
{
  unsigned int i, ii;
  eresi_Addr    virtual_eip;
  unsigned char buff[1024];
  unsigned int index_chaine;
	
  
  if (ptr == 0)
    ptr = data_ptr;
  else
    data_ptr = ptr;

  selector = selector & 0xFFFF;
  virtual_eip = ke2dbg_virt2lin_convert(selector, (unsigned int)ptr);
  ptr = virtual_eip;

  for (i=0;i<0x10 * data_win_dim;i++)
    {
      if (!ke2dbg_page_exist(virtual_eip+i))
	tab_true_byte[i] = scratch[i];
      else
	tab_true_byte[i] = ke2dbg_true_byte_bp((char *) ptr + i);
    }

  /* print data en hexa */
  for (i = 0; i < data_win_dim; i++)
    {
      index_chaine  = 0;
      index_chaine += ke2dbg_sprintf(buff, "%.4X:%.8X ", selector, ptr + i * 16);

      for (ii = 0; ii < 16; ii++)
	index_chaine += ke2dbg_sprintf(buff + index_chaine, "%.2X ", 
				tab_true_byte[i * 16 + ii] & 0xFF);
      
      for (ii = 0; ii < 16; ii++)
	index_chaine += ke2dbg_sprintf(buff + index_chaine, "%c", 
				tab_true_byte[i*0x10+ii] ? tab_true_byte[i * 16 + ii] & 0xff : '.');
      
      for (ii = 0; ii < 78; ii++)
	put_XY_char(ii + 1, data_win_y + i, buff[ii]);
    }

  ke2dbg_Refresh_DW();
  ke2dbg_display_refresh();
}


#if defined(__GNUC__)
#define DUMMY_ERR_CODE\
   _asm_("\t"\
	"pushl $0xBADCAFFE\n\t"\
);

/*TODO: BUG: ERR de prot gnrale a cause de la restauration de GS ?!? */


#define HOOK_INT_HEADER_ASM(old_hooker)\
  _asm_("\t"\
	"pushl $0xDEADBEEF\n\t"\
	"push %ds\n\t"\
	"push "\
	RING_HOOO_SEGMENT\
	"\n\t"\
	"pop %ds\n\t"\
	"push %gs\n\t"\
	"push %fs\n\t"\
	"push %es\n\t"\
        "mov ring0_ES, %es\n\t"\
        "mov ring0_FS, %fs\n\t"\
	"pusha\n\t"\
	"movl %esp, reg_stack\n\t"\
  );\
  reg_stack->reg_dummy_ret = (unsigned int)old_hooker;



#define HOOK_INT_END_NOERR_ASM(hooker)\
  _asm_("\t"\
	"popa\n\t"\
	"pop %es\n\t"\
	"pop %fs\n\t"\
	"pop %gs\n\t"\
	"cmp $0, fired\n\t"\
	"pop %ds\n\t"\
	"jz not_"hooker"\n\t"\
	"addl $4, %esp\n\t"\
	"addl $4, %esp\n\t"\
	"iret\n\t"\
"not_"hooker":\n\t"\
	"ret $0x4\n\t"\
	);


#define HOOK_INT_END_ERR_ASM(hooker)\
  _asm_("\t"\
	"popa\n\t"\
	"pop %es\n\t"\
	"pop %fs\n\t"\
	"pop %gs\n\t"\
	"cmp $0, fired\n\t"\
	"pop %ds\n\t"\
	"jz not_"hooker"\n\t"\
	"addl $4, %esp\n\t"\
	"addl $4, %esp\n\t"\
	"iret\n\t"\
"not_"hooker":\n\t"\
	"ret\n\t"\
	);
#endif

 
/**
 * handler du clavier:                             
 * fonction appelee par l'interruption declenchee  
 * par l'appuie d'une touche. appele le            
 * de touche (commande....)                        
 * BUG: scancode extended, (double octet en        
 * parametre )                                     
 */
void _ke2dbg_kbdhandle ()
{
  unsigned int oo;
  /* needed by "boucle de tempo pour ack du 8259" */
  int i;
     
  //oo = ke2dbg_keystroke_print();
  oo = ke2dbg_scancode_read();
  ke2dbg_handle_scancode((unsigned char)oo);

  if (fired)
    {
      
      if (old_data_ptr!= data_ptr)
	{
	  ke2dbg_data_dump (current_reg_stack.reg_DS, data_ptr);
	  old_data_ptr = data_ptr;
	}

      if (old_code_ptr!= code_ptr)
	{

	  ke2dbg_printf_disasm (current_reg_stack.reg_CS, code_ptr);
	  old_code_ptr = code_ptr;
	}
      
      ke2dbg_keystate_print(oo);

    }

  ke2dbg_keystate_print(oo);

  /* boucle de tempo pour ack du 8259 */
  for(i=0;i<100000;i++);


}

/**
 *
 */ 
void __declspec_naked ke2dbg_kbdhandle(void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(oldkbd);
  ke2dbg_keystroke_print();
  //_ke2dbg_kbdhandle();
  //HOOK_INT_END_NOERR_ASM("kbh");


#if defined(__GNUC__)
 _asm_("\t"	
	"popa\n\t"
	"pop %es\n\t"
	"pop %fs\n\t"
	"pop %gs\n\t"
	"cmp $0, fired\n\t"
	"pop %ds\n\t"
	"jz not_hook\n\t"
	"pushl %eax\n\t"
	"mov $0x20, %al\n\t"
	"out %al, $0x20\n\t"
	"popl  %eax\n\t"
	"addl $4, %esp\n\t" /*sucre le ret*/
	"addl $4, %esp\n\t" /*sucre le dummy err*/
	"iret\n\t"
	"not_hook:\n\t"
	"ret $0x4\n\t"
	);
#endif 

}



/** 
 * rr0d stepping function 
 * Scan keyboard from here since we are already in interrupt
 */
void ke2dbg_step (void)
{
  int ret=0;
  asm_instr my_ins;
  int eip_in_asm_win = 0;
  unsigned int i;
  unsigned char *adresse;

#if defined(__GNUC__)
  _asm_("push %eax;\n"
	"pushf;\n"
	"pop %eax;\n"
	"mov %eax, saved_eflag;\n"
	"pop %eax;\n"
	"cli;\n"
	);
#endif

  ke2dbg_screen_dump();
	
  if (fired)
    {
      adresse = (unsigned char*) ke2dbg_virt2lin_convert(reg_stack->reg_CS, reg_stack->reg_EIP);
      ret = asm_read_instr(&my_ins, (unsigned char *) adresse, 32, world.curjob->proc);
      
      debug_[0] = '0' + ret;	
      debug_[1] = 'Ä';

      switch (my_ins.instr)
	{

	case ASM_CALL:
	  strcpy(&debug_[2], "CALL");
	  debug_[6] = 0;
	  break;
	case ASM_PUSHF:
	  strcpy(&debug_[2], "pushf/d");
	  debug_[6] = 0;
	  break;
	case ASM_POPF:
	  strcpy(&debug_[2], "popf/d");
	  debug_[6] = 0;
	  break;
	case ASM_CLI:
	  strcpy(&debug_[2], "cli");
	  debug_[6] = 0;
	  break;
	case ASM_STI:
	  strcpy(&debug_[2], "int XX");
	  debug_[6] = 0;
	  break;
	case ASM_INT:
	  break;
	default:
	  strcpy(debug_, "ÄÄÄÄÄÄÄÄÄÄÄÄ");
	  debug_[10]=0;
	  ret = 0;
	  break;
	}

      ke2dbg_text_display(52, cmd_win_y-1, debug_);	  
    }
	

  do
    {
      if (fired)
	{
	  key_scan_code = (unsigned char)ke2dbg_scancode_read();
	  ke2dbg_handle_scancode(key_scan_code);
			

	  //if (old_data_ptr!= data_ptr)
	    {
	      ke2dbg_data_dump(current_reg_stack.reg_DS, data_ptr);
	      old_data_ptr = data_ptr;
	    }

	  eip_in_asm_win = 0;
	  for (i=0;i<asm_lines_adresse_n;i++)
	    if (ke2dbg_virt2lin_convert(reg_stack->reg_CS, (unsigned int)reg_stack->reg_EIP) == 
		asm_lines_adresse[i])
	      {
		eip_in_asm_win = 1;
		break;
	      }

	  if (!eip_in_asm_win && !force_disasm)
	    {
	      code_ptr = reg_stack->reg_EIP;
	      update_disasm=1;
	    }
			

	  if (update_disasm)
	    {
	      update_disasm = 0;
	      ke2dbg_printf_disasm(current_reg_stack.reg_CS, code_ptr);
	    }

	  ke2dbg_register_print(reg_stack);
	
	  if (key_scan_code)
	    ke2dbg_keystate_print(key_scan_code);
	}

      
    }while( ((key_scan_code & 0xFF) != STEP_INTO) &&
	    ((key_scan_code & 0xFF) != STEP_OVER) &&
	    ((key_scan_code & 0xFF) != RUN)	&&
	    (fired));

  switch((key_scan_code & 0xFF))
    {
    case STEP_INTO:
      ke2dbg_screen_restore(0);
      break;
    default:
      ke2dbg_screen_restore(1);
      break;
    }

#if defined(__GNUC__)
  _asm_("push %eax;\n"
	"mov saved_eflag, %eax;\n"
	"push %eax;\n"
	"popf;\n"
	"pop %eax;\n"
	);
#endif
	
  if ((key_scan_code & 0xFF) == RUN)
    {
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
      step_on = 0;
      //		fired=0;
    }

  if ( ((key_scan_code & 0xFF) == STEP_INTO) || (((key_scan_code & 0xFF) == STEP_OVER)&&(!ret)))
    {
      step_on = 1;
      //in case the sucker did popf with tf off ;)
      //humm, I love this smell
      reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
    }

  if (((key_scan_code & 0xFF) == STEP_OVER)&&(ret))
    {

      step_on = 0;
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
      ke2dbg_insert_bp(current_reg_stack.reg_CS, current_reg_stack.reg_EIP+ret, BP_ONE_SHOT);
    }


  old_reg_data = *reg_stack;

}


/*handler de la division par 0                    */
void  _ke2dbg_int0_handle (void)
{

  if (!fired)
    {
      ke2dbg_allwin_refresh();
      ke2dbg_display_refresh();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  /*
    ke2dbg_data_dump(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_register_print(reg_stack);	
  */

  fired=1;
  
  if (fired)
    {	ke2dbg_logbuf_insert("(_.-div 0  h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-div 0  ziiip -._)");	}
  
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  ke2dbg_step();

}

/**
 *
 */
void __declspec_naked ke2dbg_int0_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[0]);
  _ke2dbg_int0_handle();
  HOOK_INT_END_NOERR_ASM("int0");
};


/**
 * handler de l'interruption 1                     
 *(appelee quant le trap flag est a 1 (pas a pas) 
 *                                                
 *                                                
 */
void  _ke2dbg_int1_handle (void) 
{


  current_reg_stack = *reg_stack;
  fired = 1;
  force_disasm = 0;
  update_disasm = 1;

  ke2dbg_dr7_set(dr7_value);  
  ke2dbg_sprintf(debug_, "int1: %X", reg_stack->reg_EIP);
  ke2dbg_logbuf_insert(debug_);


  //printk("int1 enter dr7 val:%X\n", ke2dbg_dr7_get());
  /*
    if (!ke2dbg_is_hw_breakpoint(reg_stack->reg_EIP))
    {
    ke2dbg_logbuf_insert("YuuuupPP tuch DRX");
  */
  //printk("ADD: %X\n", reg_stack->reg_EIP);
  /*
    ke2dbg_data_dump(reg_stack->reg_DS, (unsigned char*)data_ptr);
    ke2dbg_printf_disasm(reg_stack->reg_CS,  (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_register_print(reg_stack);	
    step_on = 1;
    ke2dbg_step();
    }
  */
  
  /*int ret;*/
  
  if (spawn_bp)
    {
      ke2dbg_spawn_breakpoint(num_old_bp);
      spawn_bp = 0;
      if (step_on == 0)
	{
	  ke2dbg_logbuf_insert("YuuuupPP1 ");
	  return;
	}
    }
	

  if (old_id_hw_bp)
    {
      ke2dbg_idhwbp_enable(id_hw_bp);
      old_id_hw_bp = 0;
      if (step_on == 0)
	{
	  ke2dbg_logbuf_insert("YuuuupPP2 ");
	  return;
	}
	    
    }


  //printk("int1-1 enter dr7 val:%X\n", ke2dbg_dr7_get());
  /*
    ke2dbg_data_dump(reg_stack->reg_DS, (unsigned char*)data_ptr);
    ke2dbg_printf_disasm(reg_stack->reg_CS,  (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_register_print(reg_stack);	
  */
  /*respawn hw bp*/

  //printk("int1-2 enter dr7 val:%X\n", ke2dbg_dr7_get());	

  if (id_hw_bp)
    {
      ke2dbg_idhwbp_enable(id_hw_bp);
    }
	
  //printk("int1-3 enter dr7 val:%X\n", ke2dbg_dr7_get());

  /*if single step caused by hw bp*/

	
  if ((id_hw_bp=ke2dbg_hwbp_check()) != 0)
    {





      //	    ke2dbg_dr7_set(ke2dbg_dr7_get() | (3 << (id_hw_bp>>1)));
      /*
	switch(id_hw_bp)
	{
	case 1:
	ke2dbg_dr7_set(ke2dbg_dr7_get() | 0x3<<0);
	break;
	case 2:
	ke2dbg_dr7_set(ke2dbg_dr7_get() | 0x3<<2);
	break;
	case 4:
	ke2dbg_dr7_set(ke2dbg_dr7_get() | 0x3<<4);
	break;
	case 8:
	ke2dbg_dr7_set(ke2dbg_dr7_get() | 0x3<<6);
	break;
	default:
	ke2dbg_logbuf_insert("YuuuupPP HW Str4ngE");
	break;


	}
      */

      //ke2dbg_dr0_set(0);

      fired=1;
      ke2dbg_logbuf_insert("(_.-HW BP SpAWN InT 1-._)");
	    
	    
      step_on=1;
      ke2dbg_idhwbp_disable(id_hw_bp);


      old_id_hw_bp = id_hw_bp;

	    



    }
	

  //printk("int1-4 enter dr7 val:%X\n", ke2dbg_dr7_get());

  if (step_on)
    {
      if (fired)
	{	ke2dbg_logbuf_insert("(_.-int  1 h00keD-._)");	}
      else
	{	ke2dbg_logbuf_insert("(_.-int  1 ziiip -._)");	}

      ke2dbg_logbuf_print();
      ke2dbg_Refresh_AW();
      ke2dbg_display_refresh();
	


      ke2dbg_step();

      if (id_hw_bp)
	reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
    }
  else
    {
      reg_stack->reg_EFLAG= 0xfffffeff & reg_stack->reg_EFLAG;
    }

  /*TEST TEST ice 386*/

  dr7_value = ke2dbg_dr7_get();
  /*
  ke2dbg_dr6_set( 0);
  ke2dbg_dr6_set( ke2dbg_dr6_get() | (1 << 13));
  */
  //printk("int ret dr7 val:%X\n", ke2dbg_dr7_get());


}

/**
 *
 */
void __declspec_naked ke2dbg_int1_handle (void) 
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[1]);
  _ke2dbg_int1_handle();
  HOOK_INT_END_NOERR_ASM("int1");


};


/**
 * handler de l'interruption 3                     
 * gere les break point software, met en mode pas a
 * pas, ...                                        
 */ 
void  _ke2dbg_int3_handle (void) 
{

  if (!fired)
    {
      ke2dbg_allwin_refresh();
      ke2dbg_display_refresh();
    }

  fired=1;
  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;

  
/*
  __asm("push %eax\n"
	"push %ds\n"
	"pop %eax\n"
	"movl %eax, test_\n"
	"pop %eax\n"
	);

    printk("ds: %p\n", test_);

  __asm("push %eax\n"
	"push %cs\n"
	"pop %eax\n"
	"movl %eax, test_\n"
	"pop %eax\n"
	);

    printk("cs: %p\n", test_);
*/
  //printk("int3 enter dr7 val:%X\n", ke2dbg_dr7_get());

  if ( (bp_num=ke2dbg_is_breakpoint((unsigned char*)reg_stack->reg_EIP-1))!=-1)
    {
      reg_stack->reg_EIP--;
      if (reg_stack->reg_EIP!=list_breakpoints[bp_num].address)
	{
	  ke2dbg_logbuf_insert("breakpoint diff mapping...changing @");
	  list_breakpoints[bp_num].address = reg_stack->reg_EIP;
	}
		
      if ((bp_num == unmapped_bp_to_shoot) && (unmapped_bp_to_shoot!=-1))
	{
	  ke2dbg_sprintf(debug_, "ke2dbg is deleting a unwanted bp@ %.8X n %d", reg_stack->reg_EIP,bp_num);
	  ke2dbg_logbuf_insert( debug_);	

	  list_breakpoints[bp_num].address = reg_stack->reg_EIP;
	  ke2dbg_erase_breakpoint(bp_num);
	  unmapped_bp_to_shoot = -1;
	  return;
		    
	}
		
      ke2dbg_pass_breakpoint(bp_num);

      num_old_bp = bp_num;
      spawn_bp = 1;
    }


  //printk("int3-1 enter dr7 val:%X\n", ke2dbg_dr7_get());

  if ((bp_num!=-1)&&(list_breakpoints[bp_num].attrib==BP_ONE_SHOT))
      ke2dbg_erase_breakpoint(bp_num);

  //printk("int3-2 enter dr7 val:%X\n", ke2dbg_dr7_get());
  /*
    ke2dbg_data_dump(reg_stack->reg_DS, (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_printf_disasm(reg_stack->reg_CS, (unsigned char*)reg_stack->reg_EIP);
    ke2dbg_register_print(reg_stack);	
  */

  if (fired)
    {	ke2dbg_logbuf_insert("(_.-int  3 h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-int  3 ziiip -._)");	}

  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();


  //printk("int3-3 enter dr7 val:%X\n", ke2dbg_dr7_get());

  ke2dbg_step();

  //printk("int3-4 enter dr7 val:%X\n", ke2dbg_dr7_get());
  if ( spawn_bp)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
 
  //printk("int3 ret dr7 val:%X\n", ke2dbg_dr7_get());


  dr7_value = ke2dbg_dr7_get();
}

/**
 *
 */
void __declspec_naked ke2dbg_int3_handle (void) 
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[3]);
  _ke2dbg_int3_handle();
  HOOK_INT_END_NOERR_ASM("int3");

};



/**
 * handler de invalid opcode                       
 */
void  _ke2dbg_int6_handle (void)
{

  if (!fired)
    {
      ke2dbg_allwin_refresh();
      ke2dbg_display_refresh();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  fired=1;
  
  if (fired)
    {	ke2dbg_logbuf_insert("(_.-invalid op  h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-invalid op  ziiip -._)");	}
  
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  ke2dbg_step();

}

/**
 *
 */
void __declspec_naked ke2dbg_int6_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[6]);
  _ke2dbg_int6_handle();
  HOOK_INT_END_NOERR_ASM("int6");
}


/**
 * handler d' erreur de protection generale        
 * ps: l'interruption de la mort                   
 * superstitieux s'abstenir, ca porte malheur.     
 */
void  _ke2dbg_int13_handle (void)
{

  if (!fired)
    {
      ke2dbg_allwin_refresh();
      ke2dbg_display_refresh();
    }

  current_reg_stack = *reg_stack;
  force_disasm = 0;
  update_disasm = 1;

  fired=1;
  
  if (fired)
    {	ke2dbg_logbuf_insert("(_.-general prot fault  h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-general prot fault  ziiip -._)");	}


  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  ke2dbg_step();

}

/**
 *
 */
void __declspec_naked ke2dbg_int13_handle (void)
{
  fake_naked;

  HOOK_INT_HEADER_ASM(old_int[13]);
  _ke2dbg_int13_handle();
  HOOK_INT_END_ERR_ASM("int13");
}


/** 
 * handler de page fault                    
 */
void  _ke2dbg_int14_handle (void)
{
  current_reg_stack = *reg_stack;

  ke2dbg_data_dump(reg_stack->reg_DS, reg_stack->reg_EIP);
  ke2dbg_printf_disasm(reg_stack->reg_CS, reg_stack->reg_EIP);
  ke2dbg_register_print(reg_stack);	
  
  fired=0;
  
  if (fired)
    {	ke2dbg_logbuf_insert("(_.-div 14  h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-div 14  ziiip -._)");	}
  
  
  //  if (reg_stack->reg_ERROR!=4 && reg_stack->reg_ERROR!=6 && reg_stack->reg_ERROR!=7)
  {

    debug_[8]=' ';
    //  ke2dbg_logbuf_insert( debug_+9);	
#if defined(__GNUC__)
    _asm_("pushl %eax\n"
	  "movl %cr2, %eax\n"
	  "movl %eax, mycr2\n"
	  "popl %eax\n");
#endif
    /*
      hexatoascii(mycr2, debug_+9);
      debug_[18]=0;
      ke2dbg_logbuf_insert( debug_);	
    */
  }
  
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  ke2dbg_step();
  
}

/**
 *
 */
void __declspec_naked ke2dbg_int14_handle (void)
{
  fake_naked;

  HOOK_INT_HEADER_ASM(old_int[14]);
  _ke2dbg_int14_handle();
  HOOK_INT_END_ERR_ASM("int14");
}




/**
 * handler de l'interruption 80                    
 */
void  _ke2dbg_int128_handle (void)
{
  current_reg_stack = *reg_stack;
  
  ke2dbg_data_dump(reg_stack->reg_DS, reg_stack->reg_EIP);
  ke2dbg_printf_disasm(reg_stack->reg_CS, reg_stack->reg_EIP);
  ke2dbg_register_print(reg_stack);	
  
  fired=0;
  
  if (fired)
    {	ke2dbg_logbuf_insert("(_.-int128 h00keD-._)");	}
  else
    {	ke2dbg_logbuf_insert("(_.-int128 ziiip -._)");	}
  
  ke2dbg_logbuf_print();
  ke2dbg_Refresh_AW();
  ke2dbg_display_refresh();
  
  if (fired)
    reg_stack->reg_EFLAG= 0x100 | reg_stack->reg_EFLAG;
  
  ke2dbg_step();

}

/**
 *
 */
void __declspec_naked ke2dbg_int128_handle (void)
{
  fake_naked;

  DUMMY_ERR_CODE;
  HOOK_INT_HEADER_ASM(old_int[128]);
  _ke2dbg_int128_handle();
  HOOK_INT_END_NOERR_ASM("int128");
}


/**
 *
 */
int ke2dbg_init(void)
{
  unsigned  int hw_video_addess = 0;
  int dwRetVal = 0;
  unsigned int idt;
  unsigned int i;

#ifdef VIDEO_FB
  screen_adresse = SCREEN_ADRESSE;
#endif /* VIDEO_FB */

  font_x = 8;
  font_y = 12;
  font_bpp = 8;


  data_ptr = 0;
  old_data_ptr = 1;
  code_ptr = 0;
  old_code_ptr = 1;

  data_win_y   = 5;//5;
  data_win_dim = 3;//10;

  code_win_y   = data_win_y+data_win_dim+1;//11;//16;
  code_win_dim = 8;//10;
		
  buff_win_y   = code_win_y+code_win_dim+1;//17;//27;
  buff_win_dim = 4;//8;

  cmd_win_y    = buff_win_y+buff_win_dim+1;//23;//37;
  cmd_win_dim  = 3;//3;

  ke2dbg_screen_prepare();

  screen_offset_x = 0;
  screen_offset_y = 0;

  screen_bpp = 2;

  insertcmdindex = 0;
  cmdindex = 0;

  reg_stack = &current_reg_stack;
  current_reg_stack.reg_EIP = 0xC0000000;
  current_reg_stack.reg_CS = RING_HOOO_SEGMENT_VALUE;

  pagedir = (unsigned int) ke2dbg_basetp();
  Out_Debug_String("chop cr3...ok\n\r");
		
  idt = (unsigned int) ke2dbg_idt_getbase();  
  Out_Debug_String("chop idt...\n\r");

  //hexatoascii(idt,debug__);

  /*sav les valeur de es fs gs*/
#if defined(__GNUC__)
  __asm(
	"push %eax\n"
	"xor %eax, %eax\n"
	"mov %es, %ax\n"
	"mov %eax, ring0_ES\n"
	"mov %fs, %ax\n"
	"mov %eax, ring0_FS\n"
	"mov %gs, %ax\n"
	"mov %eax, ring0_GS\n"
	"pop %eax"
	);
#endif
		
  ke2dbg_hwbp_reset();

  for (i=0;i<MAX_INT;i++)
    {
      is_int_hooked[i] = 0;
      old_int[i] = 0;
      addrnewint[i] = 0;
    }

  is_int_hooked[0] = 1;
  is_int_hooked[1] = 1;
  is_int_hooked[3] = 1;
  is_int_hooked[6] = 0;
  is_int_hooked[13] = 0;

  addrnewkbd  = ((unsigned int) &ke2dbg_kbdhandle);
  addrnewint[0] = ((unsigned int) &ke2dbg_int0_handle);
  addrnewint[1] = ((unsigned int) &ke2dbg_int1_handle);
  addrnewint[3] = ((unsigned int) &ke2dbg_int3_handle);
  addrnewint[6] = ((unsigned int) &ke2dbg_int6_handle);
  addrnewint[13] =((unsigned int) &ke2dbg_int13_handle);
  addrnewint[14] =((unsigned int) &ke2dbg_int14_handle);
  addrnewint[128]=((unsigned int) &ke2dbg_int128_handle);


  Out_Debug_String("install hook...\n\r");


  ke2dbg_wpbit_reset();

#ifndef DONT_HOOK_KBD
  oldkbd =(void *) ke2dbg_idt_modify(addrnewkbd,(unsigned int*)idt+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */

  for (i=0;i<MAX_INT;i++)
    {
      if (is_int_hooked[i])
	old_int[i] =(void *) ke2dbg_idt_modify(addrnewint[i],(unsigned int*)idt+2*i);
    }
  ke2dbg_wpbit_set();

  Out_Debug_String("install hook...ok\n\r");

  ke2dbg_logbuf_clean();
  Out_Debug_String("init fini.1\n\r");

  //__asm{int 3};
  ke2dbg_console_on();
  Out_Debug_String("init fini.2\n\r");


  //ke2dbg_display_refresh();

  Out_Debug_String("init fini.\n\r");

#if 0
  init_symb();
#endif
  Out_Debug_String("symbol oki.\n\r");

  pci_list_controller();
  hw_video_addess = pci_detect_display();

  if (hw_video_addess !=0)
    {
      ke2dbg_sprintf(debug_, 
		     "Autodetect return address: 0x%.8X\n", 
		     hw_video_addess);
      Out_Debug_String(debug_);
#ifdef AUTO_HW_FB
      Out_Debug_String("Setting this video address\n");

      screen_adresse = hw_video_addess;
#endif
    }
  else
    {
      Out_Debug_String("PCI scan failed\n");
    }
	
  /* Initialize REVM interpreter */
  e2dbg_kpresence_set(1);
  revm_init();
  revm_setup(0, NULL, REVM_STATE_DEBUGGER, REVM_SIDE_SERVER);
  revm_config(KE2DBG_CONFIG);
  ke2dbg_commands_install();
  return (dwRetVal);
}

int SPY_Dynamic_Exit(void);

/**
 *
 */
void ke2dbg_cleanup(void)
{
  int dwRetVal = 0;
  int i;

  /* Dispatch to cleanup proc */
  Out_Debug_String("SPY: Closing!\n\r");

  dwRetVal = 1;

  Out_Debug_String("Spy: test unloading\n\r");

  ke2dbg_hwbp_reset();
#ifndef DONT_HOOK_KBD
  tmpkbd =(void *) ke2dbg_idt_modify( (unsigned int)oldkbd,   (unsigned int*)idt_[0]+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */

  for (i=0;i<MAX_INT;i++)
    {
      if (is_int_hooked[i])
	tmp_int[i] =(void *) ke2dbg_idt_modify( (unsigned int)old_int[i],(unsigned int*)idt_[0]+2*i);
    }

		
  SPY_Dynamic_Exit();
}

/**
 *
 */
int SPY_Dynamic_Exit(void)
{
  int i;

  tmpkbd =   (void*)ke2dbg_idt_readent((unsigned int*)idt_[0]+2*KEYB_INTER);

  for (i=0;i<MAX_INT;i++)
    {
      tmp_int[i] = (void*)ke2dbg_idt_readent((unsigned int*)idt_[0]+2*i);
    }

  /*
   * BUG: faut del les break point avant de partir: pb de
   * rémanance des CC dans les pages memoire cache penser a
   * hooked les page fault pour savoir si la page a été giclée
   * de la ram sucre les breakpoint mais sert a rien.
   */
#if 0
  for (i=0;i<NUM_BREAKPOINTS;i++)
    ke2dbg_erase_breakpoint(i);
#endif
  ke2dbg_hwbp_reset();

  if (&ke2dbg_kbdhandle == tmpkbd)
    {
#ifndef DONT_HOOK_KBD
      Out_Debug_String("SPY: Trouble in Unhooking proc!! [keyb] re-unhook\n\r");
      tmpkbd =(void *) ke2dbg_idt_modify( (unsigned int)oldkbd,   (unsigned int*)idt_[0]+2*KEYB_INTER);
#endif /* DONT_HOOK_KBD */
    }

  Out_Debug_String("SPY: Dynamic Exit\n\r");

  return(1);
}



 
 
