/*
 *
 *
 *
 *
 *
 */

#ifndef KE2DBG_H
 #define KE2DBG_H

#include <ke2dbg-vars.h>
#include <ke2dbg-config.h>
#include <libstderesi.h>

#ifdef FREEBSD
 #include <vm/vm.h>
 #include <vm/pmap.h>
#endif


typedef struct _bp_struct 
{
  unsigned long address;
  unsigned long paddress;
  unsigned char hw;
  unsigned char value;
  unsigned int attrib;
} Bp_struct;

typedef struct reg_struct 
{
  eresi_Addr reg_EDI;
  eresi_Addr reg_ESI;
  eresi_Addr reg_EBP;
  eresi_Addr reg_ESP_;
  eresi_Addr reg_EBX;
  eresi_Addr reg_EDX;
  eresi_Addr reg_ECX;
  eresi_Addr reg_EAX;
  eresi_Addr reg_ES;
  eresi_Addr reg_FS;
  eresi_Addr reg_GS;
  eresi_Addr reg_DS;
  eresi_Addr reg_dummy_ret;
  eresi_Addr reg_ERROR;
  eresi_Addr reg_EIP;
  eresi_Addr reg_CS;
  eresi_Addr reg_EFLAG;
  eresi_Addr reg_ESP;
  eresi_Addr reg_SS;
} Reg_struct;


typedef struct tagBITMAPFILEHEADER 
{ /* bmfh */
  unsigned int   bfSize; 
  short     bfReserved1; 
  short     bfReserved2; 
  unsigned int   bfOffBits; 
} BITMAPFILEHEADER; 


typedef struct tagBITMAPINFOHEADER
{ /* bmih */
  unsigned int  biSize; 
  unsigned int   biWidth; 
  unsigned int   biHeight; 
  short int   biPlanes; 
  short int   biBitCount ;
  int  biCompression; 
  int  biSizeImage; 
  unsigned int   biXPelsPerMeter; 
  unsigned int   biYPelsPerMeter; 
  int  biClrUsed; 
  int  biClrImportant; 
} BITMAPINFOHEADER; 


#define NUM_BREAKPOINTS 20
#define NUM_HW_BREAKPOINTS 4
#define HW_MAGIC_VALUE 0x100

#define BP_CLASSIC    0x1
#define BP_ONE_SHOT    0x2
#define BUFFERMAXCMD 40

#define KEYBOARD_PORT_60 0x60
#define KEYBOARD_PORT_64 0x64

#define KBENTER		-1
#define KBF11		-2
#define KBF12		-3
#define KBDEL           -4
#define KBCURSORUP      -5
#define KBCURSORDOWN    -6
#define KBLEFTALT       -7
#define KBRIGHTSHIFT    -8
#define KBPAGEUP        -9
#define KBPAGEDOWN      -10
#define KBLEFTSHIFT	-11
#define KBCURSORLEFT	-12
#define KBCURSORRIGHT	-13
#define KBESC	        -14
#define KBUP		-128

#define ke2dbg_toupper(c)  ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define ke2dbg_tolower(c)  ((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))

#define ke2dbg_screen_dump_size (80 * 25 * 2)
#define SCREEN_LINE	40
#define BITMAP_ID	0x4d42
#define BUFFERMAXLINES  80

#define PAG4M    2
#define EXISTE   1
#define NOEXISTE 0

#ifndef NULL
#define NULL 0
#endif

/* Keyboard defines */
#define CTRL_KEY	0x1d
#define F_KEY		0x21

// commands
#define READ_CONTROLLER		0x20
#define WRITE_CONTROLLER	0x60

// command bytes
#define SET_LEDS		0xED
#define KEY_RESET		0xFF

// responses from keyboard
#define KEY_ACK			0xFA	// ack
#define KEY_AGAIN		0xFE	// send again

// status register bits
#define IBUFFER_FULL		0x02
#define OBUFFER_FULL		0x01

// flags for keyboard LEDS
#define SCROLL_LOCK_BIT		(0x01 << 0)
#define NUMLOCK_BIT		(0x01 << 1)
#define CAPS_LOCK_BIT		(0x01 << 2)


int ke2dbg_insert_bp(unsigned int, unsigned int, unsigned int );
int ke2dbg_is_breakpoint(unsigned char *);
int ke2dbg_is_hw_breakpoint(unsigned char* );
int ke2dbg_erase_breakpoint(unsigned int );
int ke2dbg_pass_breakpoint(unsigned int );
int ke2dbg_spawn_breakpoint(unsigned int );
void ke2dbg_display_breakpoints(void);
unsigned char ke2dbg_true_byte_bp(unsigned char *);

//int parse_inst(unsigned int , unsigned int , insn*);

void ke2dbg_printf_disasm(eresi_Addr, eresi_Addr);

unsigned int ke2dbg_dr0_get(void);
unsigned int ke2dbg_dr1_get(void);
unsigned int ke2dbg_dr2_get(void);
unsigned int ke2dbg_dr3_get(void);
unsigned int ke2dbg_dr4_get(void);
unsigned int ke2dbg_dr5_get(void);
unsigned int ke2dbg_dr6_get(void);
unsigned int ke2dbg_dr7_get(void);

void ke2dbg_dr0_set(unsigned int value);
void ke2dbg_dr1_set(unsigned int value);
void ke2dbg_dr2_set(unsigned int value);
void ke2dbg_dr3_set(unsigned int value);
void ke2dbg_dr4_set(unsigned int value);
void ke2dbg_dr5_set(unsigned int value);
void ke2dbg_dr6_set(unsigned int value);
void ke2dbg_dr7_set(unsigned int value);

unsigned int ke2dbg_cr4_get(void);
void ke2dbg_cr4_set(unsigned int value);
void ke2dbg_hwbp_reset(void);
void ke2dbg_hwbp_set(unsigned int ad);
unsigned int ke2dbg_pageinfo_print_i(unsigned int adresse);
void ke2dbg_pageinfo_print(void);
int ke2dbg_hwbp_check(void);
void ke2dbg_hwbp_enable(int i);
void ke2dbg_hwbp_disable(int i);
void ke2dbg_idhwbp_enable(int id_hw_bp);
void ke2dbg_idhwbp_disable(int id_hw_bp);
void ke2dbg_wpbit_reset(void);
void ke2dbg_wpbit_set(void);

void ke2dbg_logbuf_clean(void);
void ke2dbg_command_insert(char *);
void ke2dbg_command_recall(int );
void ke2dbg_logbuf_insert(char *);
void ke2dbg_logbuf_print(void);
void ke2dbg_logbuf_downline(void);
void ke2dbg_logbuf_upline(void);
void ke2dbg_logbuf_downlines(void); 
void ke2dbg_logbuf_uplines(void);
void ke2dbg_logbuf_start(void);
void ke2dbg_logbuf_end(void);
void ke2dbg_logbuf_restore(void);
void ke2dbg_output(char *ptr);

void _ke2dbg_kbdhandle (void);
void _ke2dbg_int0_handle (void);
void _ke2dbg_int1_handle (void);
void _ke2dbg_int3_handle (void);
void _ke2dbg_int6_handle (void);
void _ke2dbg_int13_handle (void);
void _ke2dbg_int14_handle (void);
void _ke2dbg_int128_handle (void);
void  ke2dbg_kbdhandle(void);
void  ke2dbg_int0_handle (void);
void  ke2dbg_int1_handle (void);
void  ke2dbg_int3_handle (void);
void  ke2dbg_int6_handle (void);
void  ke2dbg_int13_handle (void);
void  ke2dbg_int14_handle (void);
void  ke2dbg_int128_handle (void);
int ke2dbg_init(void);
void ke2dbg_cleanup(void);

unsigned int* ke2dbg_idt_getbase(void);
unsigned int* ke2dbg_gdt_getbase(void);  
unsigned int ke2dbg_ldt_getbase(void);
unsigned int ke2dbg_virt2lin_convert(unsigned int , unsigned int );
void  ke2dbg_idt_print(unsigned long *);
unsigned long ke2dbg_idt_modify(unsigned int ,unsigned int *);
unsigned long ke2dbg_idt_readent(unsigned int *);

void ke2dbg_kbdreset(void);
void dummy_asm(void);
unsigned int ke2dbg_scancode_read(void);
void ke2dbg_handle_scancode(unsigned char );
char ke2dbg_scan_to_ascii(int );
unsigned int ke2dbg_keyboard_wait(void);
void ke2dbg_keyboard_read(void);
unsigned int ke2dbg_keyboard_write(unsigned char);
unsigned char  ke2dbg_keystroke_print(void);
unsigned int myke2dbg_keyboard_wait(unsigned char*);
unsigned int _ke2dbg_scancode_read(void);

unsigned int* ke2dbg_basetp(void);
unsigned int ke2dbg_pageinfo_get(unsigned int addr, unsigned int *tab_page, unsigned int *tab_page_ent, unsigned int *level);
int ke2dbg_page_exist(unsigned int );
unsigned int ke2dbg_pageinfo_set(unsigned int adresse, unsigned int data, unsigned int level);
int write_save_dirty(unsigned char* ptr, unsigned char o);
void ke2dbg_page_display(unsigned int page_ad);
unsigned int kedbg_physpage_get(unsigned int ad);
unsigned int ke2dbg_linear2physical(unsigned int ptr);
void poked(unsigned int ptr, unsigned int v);
void poked1(unsigned int ptr, unsigned int v);
void poked_cache(unsigned int ptr, unsigned int v);


unsigned char ke2dbg_inb (unsigned int Port);
unsigned short ke2dbg_inw (unsigned int Port);
unsigned int ke2dbg_inl (unsigned int Port);
void ke2dbg_outb (unsigned char Value , unsigned int Port);
void ke2dbg_outw (unsigned short Value, unsigned int Port);
void ke2dbg_outl (unsigned int Value  , unsigned int Port);

int ke2dbg_sprintf (char *outbuf,  char *fmt, ...);
int ke2dbg_isalpha(register int );
int ke2dbg_isupper(register int );
int ke2dbg_islower(register int ); 
int ke2dbg_isdigit(register int ); 
int ke2dbg_isxdigit(register int ); 
int ke2dbg_isspace(register int );
int ke2dbg_ispunct(register int ); 
int ke2dbg_isalnum(register int ); 
int ke2dbg_isprint(register int ); 
int ke2dbg_isgraph(register int ); 
int ke2dbg_iscntrl(register int ); 
int ke2dbg_isascii(register int ); 
int ke2dbg_toascii(register int ); 

int ke2dbg_atoi( char *);
char * ke2dbg_strchr( char *, int );
int ke2dbg_strcmp( char *,  char *);
size_t ke2dbg_strlen( char *);
int ke2dbg_strnicmp ( char *, char *,size_t );
int ke2dbg_stricmp (char *, char *);
char * ke2dbg_strcpy(char *,  char *);
unsigned  ke2dbg_strcspn( char *,  char *);
char *ke2dbg_strcat(char *,  char *);
char *ke2dbg_strncat(char *,  char *, unsigned int );
int ke2dbg_strncmp( char *,  char *, unsigned int );
char * ke2dbg_strncpy(char *,  char *, unsigned int );
char * ke2dbg_strpbrk( char *,  char *);
char * ke2dbg_strrchr( char *, int );
unsigned int ke2dbg_strspn( char *,  char *);
char * ke2dbg_strstr( char *,  char *);
char * ke2dbg_strtok_r(char *,  char *, char **);
void* ke2dbg_memset(void* , int , unsigned int  );
void* ke2dbg_memcpy(void* ,  void* , unsigned int );


int test(void);
void ke2dbg_screen_prepare(void);
int LoadBMP(char* , unsigned int* , unsigned int* , unsigned int *, unsigned char *);
void my_outb(unsigned short port, unsigned char value);
void ke2dbg_membase_read(void);
void ke2dbg_membase_restore(void) ;
void ke2dbg_memreset(void);
void put_XY_char(int x, int y, unsigned char a);
void print_char(unsigned char , unsigned int , unsigned int );
void Put_String(char* , unsigned int , unsigned int );
void PutString(unsigned long ,unsigned long ,char *,char );
void ke2dbg_screen_dump(void);
void ke2dbg_data_dump(eresi_Addr selector, eresi_Addr ptr);
void ke2dbg_screen_restore(int);
void ke2dbg_message_display(char *);
void WriteVCon(unsigned int ,unsigned ,char *);
void ke2dbg_line_delete(int );
void ke2dbg_ClearAW(void);
void ke2dbg_ClearDW(void);
void ke2dbg_ClearCW(void);
void cls(void);
void ke2dbg_text_display(int ,int ,char *);
void ke2dbg_Refresh_AW(void);
void ke2dbg_Refresh_DW(void);
void ke2dbg_codewin_refresh(void);
void ke2dbg_allwin_refresh(void);
void ke2dbg_cursor_top_right(void);
void ke2dbg_display_refresh(void);

#ifdef VIDEO_AA
void ke2dbg_display_start(void);
void kedbg_display_end(void);
#endif /* VIDEO_AA */

void ke2dbg_cmdkey_insert(char );
void ke2dbg_command_clear(void);
void ke2dbg_cmdkey_delete(void);

void ke2dbg_change_color(char *,char *);
void ke2dbg_console_off(void); 
void ke2dbg_console_on(void);
void ke2dbg_screen_test(void);
void ke2dbg_print_AW(char* ptr, int ligne);
void ke2dbg_print_DW(char* ptr, int ligne);
void ke2dbg_print_CW(char* ptr, int ligne);
void ke2dbg_cursor_update(unsigned int x, unsigned int y);
void ke2dbg_cursor_erase(void);

/* PCI API that must be exported */
void         pci_list_controller(void);
unsigned int pci_detect_display(void);


/* ke2dbg.c */
void ke2dbg_register_print(Reg_struct *regstack);
void ke2dbg_keystate_print (unsigned int o );
void ke2dbg_step (void);

/* rr0d commands */
void		ke2dbg_commands_install();
int             cmd_rr0d_rasta();
int             cmd_rr0d_rasta_off();
int             cmd_rr0d_set();
int             cmd_rr0d_help();
int             cmd_rr0d_insertbp();
int             cmd_rr0d_removebp();
int             cmd_rr0d_idt();
int             cmd_rr0d_bpm();
int             cmd_rr0d_loadsyms();
int             cmd_rr0d_info();
int             cmd_rr0d_listsyms();
int             cmd_rr0d_editmem();
int             cmd_rr0d_inthook();
int             cmd_rr0d_rdtschook();
int             cmd_rr0d_foolmode();
int             cmd_rr0d_unfoolmode();
int		cmd_rr0d_print();

/* Extern variables */

extern eresi_Addr	adresse_ret_1;
extern eresi_Addr	adresse_ret_2;
extern int		hook_rdtsc;
extern unsigned int	edit_mode;

extern int accessline[SCREEN_LINE];
extern char  screen_lines[39][80];
extern char  screen_colors[39][80];

extern unsigned int data_win_y;
extern unsigned int data_win_dim;
extern unsigned int code_win_y;
extern unsigned int code_win_dim;
extern unsigned int buff_win_y;
extern unsigned int buff_win_dim;
extern unsigned int cmd_win_y;
extern unsigned int cmd_win_dim;
extern unsigned int screen_offset_x;
extern unsigned int screen_offset_y;
extern unsigned int screen_bpp;
extern unsigned int code_dim_line[10];

extern int          fired;
extern unsigned int gdt_limit;
extern unsigned long idt_[2];
extern unsigned long gdt_[2];
extern unsigned long ldt_[1];
extern unsigned int data_ptr;
extern unsigned int old_data_ptr;
extern unsigned int code_ptr;
extern unsigned int old_code_ptr;
extern char debug__[80];

extern int unmapped_bp_to_shoot;
extern int bufferindex;
extern  int insertbufferindex;
extern char buff_cmd[BUFFERMAXCMD][80];
extern int insertcmdindex;
extern int cmdindex;
extern Bp_struct list_breakpoints[NUM_BREAKPOINTS];

extern unsigned char fb_hw_padding_var1[0x1000];
extern unsigned char fb_hw_flying_page1[1024*0x1000];
extern unsigned char fb_hw_padding_var2[0x1000];
extern unsigned int fb_hw_flying_page1_sav;
extern unsigned int fb_hw_flying_page1_level;

extern unsigned int cursor_X;
extern unsigned int cursor_Y;
extern unsigned int cursor_old_color;
extern unsigned int cursor_color;

extern unsigned int font_width;
extern unsigned int font_height;
extern unsigned int font_x;
extern unsigned int font_y;
extern unsigned int font_bpp;
extern unsigned int screen_adresse ;
extern unsigned char font_data[];

extern unsigned int pagedir;
extern unsigned int saved_wp_bit;

extern Reg_struct *reg_stack;
extern Reg_struct current_reg_stack;
/* BUG /!\ pour nux, 0x18 */

/* keyboard.c */
extern int CommandLength;
extern char Command[256];
extern unsigned int rasta_mode;
extern unsigned int fOOl_mode;
extern unsigned int fOOl_mode_color_shift;
extern unsigned int first_asm_line;
extern unsigned int update_disasm;
extern unsigned int force_disasm;
extern char *table_command[];
extern long back_disasm (unsigned int , int ); 
extern void *old_int[MAX_INT];
extern void *tmp_int[MAX_INT];
extern unsigned long addrnewint[MAX_INT];
extern int is_int_hooked[MAX_INT];
extern Reg_struct *reg_stack;
extern unsigned int ctrl_key;
extern unsigned int shift_key;
extern unsigned int alt_key;

/* strings.c */
int	strcmp(const char *p1, const char *p2);
char    *strncat(char *s1, const char *s2, size_t n);
char	*strncpy(char *s1, const char *s2, size_t n);
int     strncmp(const char  *s1, const char *s2, size_t n);
char    *strcat(char *dest, const char *src);
char	strtok_r(char *s, const char *delim, char **save_ptr);
void    bzero(void *s, size_t len);

#define	CMD_RASTA		"rr0d-rasta"
#define	CMD_RASTA_OFF		"rr0d-norasta"
#define	CMD_FOOL		"rr0d-f00l"
#define	CMD_FOOL_OFF		"rr0d-nof00l"
#define	CMD_IDT			"rr0d-idt"
#define	CMD_BPM			"rr0d-bpm"
#define	CMD_RINFO		"rr0d-rinfo"
#define	CMD_EDITMEM		"rr0d-editmem"
#define	CMD_INTHOOK		"rr0d-inthook"
#define	CMD_RDTSCHOOK		"rr0d-rdtschook"

#endif /* KE2DBG_H */
