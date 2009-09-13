/**
* @file libe2dbg/kernel/video.c
 *
 */
#include "ke2dbg.h"
#include "ke2dbg-font256.h"

unsigned int rasta_mode=0;
unsigned int fOOl_mode = 0;
unsigned int fOOl_mode_color_shift=0;
unsigned int cursor_X=0;
unsigned int cursor_Y=0;
unsigned int cursor_old_color=0;
unsigned int cursor_color='0';

#define COLORSET1 0x12
#define COLORSET2 0x50

int insline=40;
char COLOR=COLORSET1;

unsigned int data_win_y;
unsigned int data_win_dim;
unsigned int code_win_y;
unsigned int code_win_dim;
unsigned int buff_win_y;
unsigned int buff_win_dim;
unsigned int cmd_win_y;
unsigned int cmd_win_dim;
unsigned int screen_offset_x;
unsigned int screen_offset_y;
unsigned int screen_bpp;
unsigned int code_dim_line[10];
unsigned int font_width;
unsigned int font_height;
unsigned int font_x;
unsigned int font_y;
unsigned int font_bpp;
unsigned int screen_adresse ;
unsigned char ke2dbg_screen_dump_data[ke2dbg_screen_dump_size];

char  screen_lines[39][80];
char  screen_colors[39][80];
int   accessline[SCREEN_LINE];
int   CommandLength=0;
char  Command[256];


/**
 *
 */
void ke2dbg_screen_prepare(void)
{
  unsigned int i, ii;

  for (i=0;i<cmd_win_y+cmd_win_dim;i++)
    for (ii=0;ii<80;ii++)
    {
      screen_lines[i][ii]=' ';
      screen_colors[i][ii]=' ';
    }

  for (i=0;i<cmd_win_y/*+cmd_win_dim*/;i++)
    {
      screen_lines[i][0] = '³';   screen_colors[i][0] = '1';
      screen_lines[i][79] = '³';      screen_colors[i][79] = '1';
    }

  for (i=0;i<80;i++)
    {
      screen_lines[0][i] = 'Ä';
      screen_colors[0][i] = '1';

      screen_lines[data_win_y-1][i] = 'Ä';
      screen_colors[data_win_y-1][i] = '1';

      screen_lines[code_win_y-1][i] = 'Ä';
      screen_colors[code_win_y-1][i] = '1';

      screen_lines[buff_win_y-1][i] = 'Ä';
      screen_colors[buff_win_y-1][i] = '1';

      screen_lines[cmd_win_y-1][i] = 'Ä';
      screen_colors[cmd_win_y-1][i] = '1';

    }

  screen_lines[0][0] = 'Ú';
  screen_lines[0][79] = '¿';

  screen_lines[data_win_y-1][0] = 'Ã';
  screen_lines[data_win_y-1][79] = '´';
  
  screen_lines[code_win_y-1][0] = 'Ã';
  screen_lines[code_win_y-1][79] = '´';
  
  screen_lines[buff_win_y-1][0] = 'Ã';
  screen_lines[buff_win_y-1][79] = '´';
  
  screen_lines[cmd_win_y-1][0] = 'À';//'Ã';
  screen_lines[cmd_win_y-1][79] = 'Ù';//'´';

  ke2dbg_memcpy(&screen_lines[1][1],  "EAX=", 4);
  ke2dbg_memcpy(&screen_lines[1][14], "EBX=", 4);
  ke2dbg_memcpy(&screen_lines[1][27], "ECX=", 4);
  ke2dbg_memcpy(&screen_lines[1][40], "EDX=", 4);
  ke2dbg_memcpy(&screen_lines[1][53], "EDI=", 4);

  ke2dbg_memcpy(&screen_lines[2][1],  "ESI=", 4);
  ke2dbg_memcpy(&screen_lines[2][14], "EBP=", 4);
  ke2dbg_memcpy(&screen_lines[2][27], "ESP=", 4);
  ke2dbg_memcpy(&screen_lines[2][40], "EIP=", 4);

  ke2dbg_memcpy(&screen_lines[3][1],  "CS =", 4);
  ke2dbg_memcpy(&screen_lines[3][14], "DS =", 4);
  ke2dbg_memcpy(&screen_lines[3][27], "ES =", 4);
  ke2dbg_memcpy(&screen_lines[3][40], "SS =", 4);
  ke2dbg_memcpy(&screen_lines[3][53], "FS =", 4);
  ke2dbg_memcpy(&screen_lines[3][66], "GS =", 4);

  for (i = 1; i < 14; i++)
    {
      for (ii=0;ii<data_win_dim;ii++)
	screen_colors[data_win_y+ii][i] = '2';
      for (ii=0;ii<code_win_dim;ii++)
	screen_colors[code_win_y+ii][i] = '2';

      if (i==4)
	i++;
    }

  for (i=0;i<6;i++)
    {
      for (ii=1;ii<4;ii++)
	{
	  screen_colors[1][13*i+ii] = '8';
	  screen_colors[2][13*i+ii] = '8';
	  screen_colors[3][13*i+ii] = '8';
	}
    }

}



/**
 *------------------------------------------------------------------
 *- bool LoadBMP(char*, GLfloat, GLfloat) --------------------------
 *------------------------------------------------------------------
 *- Description: This function loads a Windows Bitmap into the	   -
 *-		 TEXTURE class object that it represents.          -
 *------------------------------------------------------------------
 *- Big thanks to NeHe for this one				   -
 *------------------------------------------------------------------
 */
int LoadBMP(char* file_data, unsigned int* width, unsigned int* height, unsigned int *font_bpp, unsigned char *data)
{

  *width = 2048;
  *height = 12;
  *font_bpp = 8;



  //	Out_Debug_String("Loaded  correctly \n\r");
	
  return 1;
}

/**
 * affiche un caractere a l'ecran a la position    
 * x y (unité: le caractere)                       
 * si ca déborde, ben ca débordera.                
 * les couleur sont gérées la aussi                
 *
 * @param a
 * @param x
 * @param y
 */

#ifdef VIDEO_FB
void print_char(unsigned char a, unsigned int x, unsigned int y)
{

  unsigned int x_pix, y_pix, x1, y1;
  int char_font_x, char_screen_x;
  int font_next_line = 256 * font_x;
  unsigned int color;
  unsigned int byte_per_pixel = font_bpp>>3;
  int tmp_decal_font_x=0;
  int tmp_decal_screen_x=0;
  int tmp_font_y_m_y=0;
  //unsigned char debug[80];


  x1 = x - screen_offset_x;
  y1 = y - screen_offset_y;

  //__asm{int 3};
//  font_x = 8;
/*
ke2dbg_sprintf(debug, "%.8X %.8X %.8X\n\r", font_x, x1, y1);
Out_Debug_String(debug);
*/


  char_font_x = font_x * (int)a;
  char_screen_x = font_x * x + font_y * y * SCREEN_MAX_X;

	//__asm{int 3};
//	ke2dbg_sprintf(debug, "->%X %X \n\r", font_x, font_y);
//  Out_Debug_String(debug);
	
  for (x_pix = 0; x_pix < font_x; x_pix++)
    {
      tmp_decal_font_x = char_font_x + x_pix;
      tmp_decal_screen_x = char_screen_x + x_pix;
      for (y_pix = 0; y_pix < font_y; y_pix++)
	{
	  tmp_font_y_m_y = (font_y-y_pix)*SCREEN_MAX_X;
	  color =  (font_data[(tmp_decal_font_x + y_pix*font_next_line)*byte_per_pixel+0]>>3);


//	    ke2dbg_sprintf(debug, "mode %.8X %.8X \n\r", rasta_mode, fOOl_mode);
//  Out_Debug_String(debug);


	  if (rasta_mode)
	    {
	      /*Rasta Mode*/
	      switch(color)
		{
		case 0:
		  break;

		default:
		  switch (x%3)
		    {
		    case 0: color = 0x0780; break;
		    case 1: color = 0xf800; break;
		    case 2: color = 0xffe0; break;
		    }
					
		  break;

		}
	    }
	  if (fOOl_mode)
	    {
	      /*f001 Mode*/ 
	      /* 0 blk, 1 darkbl, 2 darkgr, 3 cyan, 4 red, 5 purpur, 6 orange, 7 lgray, 8 dgray, 
		 9 lblue, 10 lgrn, 11 lblu, 12 lred, 13 lping, 14 yell, 15 white */
	      
	      switch ( ((x+fOOl_mode_color_shift)>>2)%8)
		{
		case 0: color = 0x1; break;
		case 1: color = 0x9; break;
		case 2: color = 0x3; break;
		case 3: color = 0xb; break;
		case 4: color = 0xf; break;
		case 5: color = 0xb; break;
		case 6: color = 0x3; break;
		case 7: color = 0x9; break;
		}
	      
	      
	    }

	  if (!rasta_mode && !fOOl_mode)
	    
	    {
//	    ke2dbg_sprintf(debug, "normal mode %.8X %.8X %.8X %X\n\r", a, x1, y1, color);
//Out_Debug_String(debug);

	      /*Normal mode*/
	      if ((x1<80) && (y1<40))
		{
		  if (!color)
		    {
		    if (x1 == cursor_X && y1 == cursor_Y) 
		      color = 0xc618;
		    }
		  else
		    {
		      switch(screen_colors[y1][x1])
			{
			case 0:
			  break;
			case ' ': color = 0xc618; break;
			case '1': color = 0x0780; break;
			case '2': color = 0x9CD3; break;
			case '8': color = 0x44b0; break;
			default:
			  break;
			  
			}
		    }
		}
	      
	    }
	  /*
	  #ifdef BYTE_PER_PIXEL_HALF
	  *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif


	  #ifdef BYTE_PER_PIXEL_ONE
	  *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif
	  #ifdef BYTE_PER_PIXEL_TWO
	  *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	  #endif
	  #ifdef BYTE_PER_PIXEL_THREE
	  *((char*)screen_adresse+0+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	  #endif
	  */
	  //__asm{int 3};
	  #ifdef BYTE_PER_PIXEL_FOUR
	  #ifdef HW_FB
	  poked1(screen_adresse+4*(tmp_decal_screen_x + (tmp_font_y_m_y)) , color);
	  #else
	  *((int*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	  #endif
	  #endif

	  /*
	  switch(screen_bpp)
	    {
	    case 1:
	      *((char*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	      break;
	    case 2:
	      *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;
	    case 3:
	      *((char*)screen_adresse+0+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=color&0xff;
	      *((char*)screen_adresse+1+2*(tmp_decal_screen_x + (tmp_font_y_m_y)))=(color>>8)&0xff;
	      break;
	    case 4:
	      *((int*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;

	    default:
	      *((short*)screen_adresse+(tmp_decal_screen_x + (tmp_font_y_m_y)))=color;
	      break;
	  
	    }
	  */	


	}
    }
	
}
#endif

#ifdef VIDEO_AA
#define TEXT_MODE_ADDRESS     0xC00B8000

unsigned char videobaselow;
unsigned char videobasehigh;

/**
 *
 */
void ke2dbg_membase_read(void)
   {
#ifdef ATT_ASM
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0c,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "inb  %dx,%al\n\t"
      "movb %al,(videobasehigh);  \n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0d,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "inb  %dx,%al\n\t"
      "movb %al,(videobaselow);\n\t"
      );
#endif
   };

/**
 *
 */   
void ke2dbg_membase_restore(void) 
   {
#ifdef ATT_ASM
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0c,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "movb (videobasehigh),%al  \n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d4,%dx\n\t"
      "movb $0x0d,%al\n\t"
      "outb %al,%dx\n\t"
      "movw $0x3d5,%dx\n\t"
      "movb (videobaselow),%al  \n\t"
      "outb %al,%dx\n\t"
      );
#endif
   };

/**
 *
 */
void ke2dbg_memreset(void)   
   {
#ifdef ATT_ASM     
   __asm("\n\t"
      "movw $0x3d4,%dx\n\t"
      "movw $0x000c,%ax\n\t"
      "outw %ax,%dx\n\t"
      "movw $0x000d,%ax\n\t"
      "outw %ax,%dx\n\t"
      );
#endif
     
   };

/**
 *
 */
void ke2dbg_display_start(void)
{

  //printk("read...");
  ke2dbg_membase_read();
  //printk("mem to 0...");
  ke2dbg_memreset();
};

/**
 *
 */
void kedbg_display_end(void)
{

  //printk("mem base...");
  ke2dbg_membase_restore();	  
};


/**
 * @param a
 * @param x
 * @param y
 */
void print_char(unsigned char a, unsigned int x, unsigned int y)
{
  char *ptr=(char *) TEXT_MODE_ADDRESS/*+80*10*2*/;
  int color=0;

  if ((y>25)||(x>80))
    return; 

  //ke2dbg_display_start();

  ptr=ptr+(2*80*y);     
  ptr=ptr+(x*2);

  *ptr=a;
  ptr++;

  if (rasta_mode)
    {
      /*Rasta Mode*/
      switch (x%3)
	{
	case 0: color = 0x4; break;
	case 1: color = 0xE; break;
	case 2: color = 0xA; break;
	}
      
      
    }

  if (fOOl_mode)
    {
      /*f001 Mode*/ 
      /* 0 blk, 1 darkbl, 2 darkgr, 3 cyan, 4 red, 5 purpur, 6 orange, 7 lgray, 8 dgray, 
	 9 lblue, 10 lgrn, 11 lblu, 12 lred, 13 lping, 14 yell, 15 white */
      
      switch ( ((x+fOOl_mode_color_shift)>>2)%8)
 	{
 	case 0: color = 0x1; break;
 	case 1: color = 0x9; break;
 	case 2: color = 0x3; break;
 	case 3: color = 0xb; break;
 	case 4: color = 0xf; break;
 	case 5: color = 0xb; break;
 	case 6: color = 0x3; break;
 	case 7: color = 0x9; break;
 	}  
    }

  /*Normal mode*/
  if (!rasta_mode && !fOOl_mode && x < 80 && y < 40)
    switch (screen_colors[y][x])
      {
      case 0:
	break;
      case ' ': 
	color = 0x7; 
	break;
      case '0': 
	color = 0xc7; 
	break;
      case '1': 
	color = 0x3; 
	break;
      case '2': 
	color = 0x8; 
	break;
      case '8': 
	color = 0x2; 
	break;
      default:
	break;
      }
    
  *ptr = (char) color;
}

#endif /* VIDEO_AA */


/**
 * Draw a string at position x-y 
 *
 * @param string
 * @param x
 * @param y
 *
 */
void Put_String(char* string, unsigned int x, unsigned int y)
{
  unsigned int i;

  for (i=0;i<ke2dbg_strlen(string);i++)
    print_char(string[i], x+i,y);
}

/** 
 * Higher level API for printing on screen 
 * @param y
 * @param x
 * @param ptr
 * @param attrib
 *
 */ 
void PutString(unsigned long y,unsigned long x,char *ptr,char attrib)
{
#ifdef VIDEO_FB
  Put_String(ptr, x+screen_offset_x, y+screen_offset_y);
#endif
#ifdef VIDEO_AA
  Put_String(ptr, x, y);
#endif
}

/** 
 * Save content of the screen for later restoration 
 */
void ke2dbg_screen_dump(void)
{
#ifdef VIDEO_AA 
  int i;
  unsigned int *ptr=(unsigned int *) TEXT_MODE_ADDRESS;

  for (i=0;i<ke2dbg_screen_dump_size/sizeof(unsigned int);i++)
    ke2dbg_screen_dump_data[i] = ptr[i];
#endif
}


/** 
 * Restore saved screen image 
 * @param do_work
 */
void ke2dbg_screen_restore(int do_work)
{
#ifdef VIDEO_AA 
  int i;
  unsigned int *ptr;

  ptr = (unsigned int *) TEXT_MODE_ADDRESS;
  
  if (!do_work)
    return;
  for (i=0;i<ke2dbg_screen_dump_size/sizeof(unsigned int);i++)
    ptr[i] = ke2dbg_screen_dump_data[i];
#endif
}

/**
 * @param x
 * @param y
 * @param a
 */
void put_XY_char(int x, int y, unsigned char a)
{
  screen_lines[y][x]=a;
}

               
/** 
 * Display a message 
 * @param ptr
 */
void ke2dbg_message_display(char *ptr) 
{
  int x=1;

  accessline[cmd_win_y+1]=1;
  while (*ptr!=0)
    {
      accessline[x]=1;
      /* screen_line24[x]=*ptr; */
      screen_lines[cmd_win_y+1][x]=*ptr;

      x++;
      ptr++;
    };

}

/**
 * @param x
 * @param y
 * @param ptr
 */
void WriteVCon(unsigned int x,unsigned y,char *ptr)
{
  if ((x<SCREEN_LINE) && (y<80) && ((ke2dbg_strlen(ptr)+y)<80)) 
    {
      strcpy(&screen_lines[x][y], ptr);     
      accessline[x]=1;
    }
}
 
/**
 * Clear a buffer line 
 *
 * @param screen_line
 */
void ke2dbg_line_delete(int screen_line)     
{
  int x;
  char *ptr;
  accessline[screen_line]=1;
  ptr=&screen_lines[screen_line][0];
  for (x=0;x<80;x++)
    {
      *ptr = ' ';
      ptr++;
    }
}

/** 
 * Clear A window 
 */
void ke2dbg_ClearAW()             
{
  unsigned int x;
  for (x=buff_win_y;x<=buff_win_y + buff_win_dim;x++)
    ke2dbg_line_delete(x);
}

/** 
 * Clear dump window 
 */
void ke2dbg_ClearDW()             
{
  unsigned int x;
  for (x=data_win_y;x<=data_win_y + data_win_dim;x++)
    ke2dbg_line_delete(x);
}


/**
 * Clear dump window 
 */
void ke2dbg_ClearCW()             
{
  unsigned int x;
  for (x=code_win_y;x<=code_win_y + code_win_dim;x++)
    ke2dbg_line_delete(x);
}

/** 
 * Clear Screen 
 */
void cls()                 
{
  insline=40;
  ke2dbg_ClearAW();
  ke2dbg_ClearDW();
}

/** 
 * Display text in a x,y position in Console 
 *
 * @param x
 * @param y
 * @param texto
 *
 */
void ke2dbg_text_display(int x,int y,char *texto) 
{
  char *buf=(char *) &screen_lines[y][x];
  char *tmp=texto;
  accessline[y]=1;
  while (*tmp!=0) 
    {
      *buf=*tmp;
      buf++;tmp++;
      x++;
      if (x>78)
	break;
    };
}

/**
 * @param ptr
 * @param ligne
 */
void ke2dbg_print_AW(char* ptr, int ligne)
{
  ke2dbg_text_display(1,buff_win_y+ligne,ptr);
}

/**
 * @param ptr
 * @param ligne
 */
void ke2dbg_print_DW(char* ptr, int ligne)
{
  ke2dbg_text_display(1,data_win_y+ligne,ptr);
}

/**
 * @param ptr
 * @param ligne
 */
void ke2dbg_print_CW(char* ptr, int ligne)
{
  ke2dbg_text_display(1,code_win_y+ligne,ptr);
}


/**
 * rafraichi la fenetre de log                     
 */
void ke2dbg_Refresh_AW()
{
  unsigned int i;
  for (i=buff_win_y;i<=buff_win_y + buff_win_dim;i++)
    accessline[i]=1;
}

/**
 * rafraichi la fenetre de data                    
 */

void ke2dbg_Refresh_DW()
{
  unsigned int i;
  for (i=data_win_y;i<data_win_y + data_win_dim;i++)
    accessline[i]=1;
}

/**
 * rafraichi la fenetre de code                    
 */
void ke2dbg_codewin_refresh()
{
  unsigned int i;
  for (i=code_win_y;i<code_win_y + code_win_dim;i++)
    accessline[i]=1;
}

/**
 * rafraichi tout                                  
 */

void ke2dbg_allwin_refresh()
{
  unsigned int i;
  for (i=0;i<SCREEN_LINE;i++)
    accessline[i]=1;
}

/**
 *
 */
void ke2dbg_cursor_top_right(void)
{
  ke2dbg_outb(0xC, 0x3d4); 
  ke2dbg_outb(0x0, 0x3d5); 
  ke2dbg_outb(0xd, 0x3d4); 
  ke2dbg_outb(0x0, 0x3d5); 
}
/*end test :)*/



/**
 *  Look for a modified lines and rewrite them 
 */
void ke2dbg_display_refresh()       
{
  unsigned long screen_line = 0;

#ifdef VIDEO_AA
  ke2dbg_cursor_top_right();
  ke2dbg_display_start();
#endif /* VIDEO_AA */

  for (screen_line = 0; screen_line < SCREEN_LINE; 
      screen_line++)
    {
      if (accessline[screen_line])
	{
	  if (insline != screen_line) 
	    PutString(screen_line, 0, 
		      &screen_lines[screen_line][0],
		      COLOR);
	  else     
	    PutString(screen_line, 0,
		      &screen_lines[screen_line][0],
		      COLORSET2);
      };
      accessline[screen_line] = 0;	
    }

#ifdef VIDEO_AA
    kedbg_display_end();
#endif /* VIDEO_AA */

}

/**
 * @param objet
 * @param val
 */
void ke2dbg_change_color(char *object,char *val)
{
}

/**
 *
 */
void ke2dbg_console_off()
{

} 

/**
 *
 */
void ke2dbg_console_on()
{
  int x;

  /* We want to modify all lines */
  for (x = 0; x < SCREEN_LINE; x++) 
    accessline[x] = 1; 
  ke2dbg_display_refresh();
}

/**
 *
 */
void ke2dbg_screen_test()
{

  int i;


  for (i=0;i<28;i++)
    Put_String(&screen_lines[i][0], 60, 70+i);


}

/**
 * @param x
 * @param y
 */
void ke2dbg_cursor_update(unsigned int x, unsigned int y)
{
  if (y<data_win_y)
    y = data_win_y;

  if (x<15)
    x = 15;
  
  if (y > data_win_y + data_win_dim - 1)
    y = (unsigned int )(data_win_y + data_win_dim-1);

  if (x > 80 - 1)
    x = 80 - 1;

  if ((x < 80) && (y < 40))
    {
      accessline[y] = 1;
      accessline[cursor_Y] = 1;
      screen_colors[cursor_Y][cursor_X] = (char)cursor_old_color;

      if (x < 63)
	if (!((x + 1) % 3))
	  x = (x > cursor_X ? x + 1 : x - 1);

      cursor_old_color = screen_colors[y][x];
      screen_colors[y][x] = (char) cursor_color;
      cursor_X = x;
      cursor_Y = y;
    }

  ke2dbg_display_refresh();
}

/**
 *
 */
void ke2dbg_cursor_erase()
{
  if ((cursor_X < 80) && (cursor_Y < 40))
    {
      accessline[cursor_Y]=1;
      screen_colors[cursor_Y][cursor_X] = (char) cursor_old_color;
    }
  ke2dbg_display_refresh();
}

/**
 * @param str
 */
void	ke2dbg_output(char *str)
{
  ke2dbg_logbuf_insert(str);
  ke2dbg_logbuf_print();
  ke2dbg_display_refresh();  
}
