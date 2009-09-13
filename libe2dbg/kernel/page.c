/**
* @file libe2dbg/kernel/page.c
 */
#include "ke2dbg.h"


unsigned int pagedir;
int init_p=0;
unsigned int saved_wp_bit=0;
unsigned char  flying_page1[0x4000] = {'o'};




/**
 * renvoie l'adresse du repertoire des pages       
 * /!\ dépendant de l'os pour l'adresse base du   
 * kernel: sous win c2690000 sous nux c0000000     
 */
unsigned int *ke2dbg_basetp(void)
{

#if defined(__GNUC__)
  _asm_("\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, pagedir\n"
	);
#elif defined(_MSC_VER)
  __asm{
        mov eax, cr3
	mov pagedir, eax
      };
#endif

  /* /!\ BUG KERNEL BASE */
  pagedir += KERNEL_B;
  return (unsigned int*)pagedir; 
}



/**
 * affiche (plus) les info des tables              
 */
void ke2dbg_pageinfo_print(void)
{
  unsigned int *page_tab;
  int i;
  unsigned int ke2dbg_pageinfo_print;
  unsigned int adr;
  unsigned int info;

  page_tab = ke2dbg_basetp();
  for (i=0;i<1024;i++)
    {
      ke2dbg_pageinfo_print = page_tab[i];		
      /* KERNEL_B */
      adr = ke2dbg_pageinfo_print & 0xFFFFF000;
      info =ke2dbg_pageinfo_print & 0x00000FFF;

      if (!(info & 1))
	continue;
#if 0
      printf("%.8X %.8X %.3X %.8X ", ke2dbg_pageinfo_print, adr, info, i*0x00400000);
      printf("p:%d ", (info&1)?1:0); /* p */
      printf("r:%d ", (info&2)?1:0); /* r */
      printf("a:%d ", (info&0x20)?1:0); /* access */
      printf("mode:%d ", (info&0x4)?1:0); /* mode */
      printf("\n");
#endif

    }

}



/**
 * renvoie l'attribut de la page et affecte le niveau:
 * (si level!=NULL) 0: merdouille; 1: 1er niveau; 2: 2 eme niveau
 *
 * @param adresse
 * @param tab_page
 * @param tab_page_entry
 * @param level
 * @return
 */
unsigned int ke2dbg_pageinfo_get(unsigned int adresse, unsigned int *tab_page, 
				 unsigned int *tab_page_entry, unsigned int *level)
{
 

#if defined FREEBSD
  /*TODO: non quand meme, faudrait faire qq chose pour les *bsd*/

  if (level)
    *level=0;

  if (tab_page)
     *tab_page = (unsigned int)PTD;

  if (!(PTD[adresse >> 22] & 1))
      return 0;

  if (PTD[adresse >> 22]&0x80)
    {
      if (level)
	      *level=1;
      return PTD[adresse >> 22];
    }

  if (level)
    *level=2;

  if (!(PTmap[adresse >> 12] & 1))
    return 0;

  if (tab_page_entry)
    *tab_page_entry = (unsigned int)PTmap;


  return PTmap[adresse >> 12];
	
  //return 1;		  
#else
  unsigned int *page_tab;
  unsigned int *sub_page_tab;
  int page_dir;
  int page;
  unsigned int ke2dbg_pageinfo_print;
  unsigned int sub_ke2dbg_pageinfo_print;
  unsigned int tmp;


  if (level)
    *level=0;

  page_tab = ke2dbg_basetp();
  page_dir = (adresse>>22) & 0x3FF; 
  ke2dbg_pageinfo_print = page_tab[page_dir];

  if (tab_page)
    {
      tmp = (unsigned int)(ke2dbg_pageinfo_print & 0xFFFFF000)+KERNEL_B;
      *tab_page = (unsigned int)tmp;
    }

  if (!(ke2dbg_pageinfo_print&1))
    return 0;

  if (ke2dbg_pageinfo_print&0x80)
    {
      if (level)
	*level=1;
      return ke2dbg_pageinfo_print;
    }

  if (level)
    *level=2;

  tmp = (ke2dbg_pageinfo_print & 0xFFFFF000)+KERNEL_B;
  sub_page_tab = (unsigned int*)tmp;

  if (tab_page_entry)
    *tab_page_entry = (unsigned int)sub_page_tab;

  page= (adresse>>12) & 0x3FF; 

  sub_ke2dbg_pageinfo_print = sub_page_tab[page];

  return sub_ke2dbg_pageinfo_print;
#endif
}

/**
 * renvoie si une page est présente en memoire     
 *
 * @param adresse
 * @return
 */
int ke2dbg_page_exist(unsigned int adresse)
{
  return ke2dbg_pageinfo_get(adresse, NULL, NULL, NULL) & 1;
}

/**
 * @param adresse
 * @param data
 * @param level
 * @return
 */
unsigned int ke2dbg_pageinfo_set(unsigned int adresse, unsigned int data, unsigned int level)
{
  unsigned int *page_tab;
  unsigned int *sub_page_tab;
  unsigned int ke2dbg_pageinfo_print;

  ke2dbg_pageinfo_print = ke2dbg_pageinfo_get(adresse,
			    (unsigned int*)&page_tab,
			    (unsigned int*)&sub_page_tab,
			    NULL);

  if (!ke2dbg_pageinfo_print)
    return 0;

  if (level==0)
    return 0;

  if (level==1)
    {
      page_tab[(adresse>>22) & 0x3FF]=data;
      return 0x1337;
    }

  if (level==2)
  {
    sub_page_tab[(adresse>>12) & 0x3FF]=data;
  }
  return 0x1337;
}

/**
 * @param ptr
 * @return
 */
unsigned int ke2dbg_linear2physical(unsigned int ptr)
{
  unsigned int info_page_src;
  info_page_src = ke2dbg_pageinfo_get(ptr, NULL, NULL, NULL );
  return (info_page_src & 0xfffff000) + (ptr & 0xfff);
}


/**
 * @param ptr
 */
void ke2dbg_page_display(unsigned int ptr)
{
  unsigned char debug[80];
  unsigned int page_ad;


  page_ad = ke2dbg_pageinfo_get(ptr, NULL, NULL, NULL );

  if (!(page_ad & 0x1))
    {
      ke2dbg_output("Not present");
      return;
    }
  ke2dbg_sprintf(debug, "%.8X %.8X %s %s %s %s %s %s", 
	  ptr, 
	  page_ad, 
	  (page_ad & 0x2)?"R/W":"R",
	  (page_ad & 0x4)?"U":"S",
	  (page_ad & 0x20)?"a":"!a",
	  (page_ad & 0x40)?"d":"!d",
	  (page_ad & 0x80)?"4M":"4K",
	  (page_ad & 0x100)?"G":"L");
  ke2dbg_output(debug);

}

/**
 *
 */
void ke2dbg_wpbit_reset(void)
{
#if defined(__GNUC__)
  _asm_(
	"push %eax\n\t"
	"movl %cr0,%eax\n\t" 
	"movl %eax, saved_wp_bit\n\t"
	"andl $0xFFFEFFFF,%eax\n\t"   
	"movl %eax,%cr0\n\t"
	"pop  %eax\n\t"
	);
#elif defined(_MSC_VER)
  _asm{
      push eax
      mov  eax, cr0
      mov  saved_wp_bit, eax
      and   eax, 0xFFFEFFFF
      mov  cr0, eax
      pop  eax
      };
#endif


  saved_wp_bit = saved_wp_bit & 0x00010000;
}

/**
 *
 */
void ke2dbg_wpbit_set(void)
{

#if defined(__GNUC__)
  _asm_(
	"push %eax\n\t"
	"movl %cr0,%eax\n\t" 
	"andl $0xFFFEFFFF, %eax\n\t"   
	"orl  saved_wp_bit, %eax\n\t"
	"movl %eax,%cr0\n\t"
	"pop  %eax\n\t"
	);
#elif defined(_MSC_VER)
  _asm{
    push eax
      mov  eax,cr0
      and  eax, 0xFFFEFFFF
      or   eax, saved_wp_bit
      mov  cr0,eax
      pop  eax
      };
#endif


}


/**
 * @param ptr
 * @param o
 * @return
 */
int write_save_dirty(unsigned char* ptr, unsigned char o)
{

  unsigned int info_page_src;
  unsigned int level;

  info_page_src = ke2dbg_pageinfo_get((unsigned int)ptr, NULL, NULL, &level );  
  ke2dbg_wpbit_reset();
  *ptr  = o;  

  ke2dbg_pageinfo_set((unsigned int)ptr, info_page_src, level);
  ke2dbg_wpbit_set();
  return 0;
}

/**
 * @param ad
 * @return
 */
unsigned int kedbg_physpage_get(unsigned int ad)
{
  unsigned char debug[80];
  unsigned int i;
  unsigned int info_page_src;
  unsigned int level;

  ad &= 0xFFFFF000;
  for (i=0;i<0xFFFF0000;i+=0x1000)
    {
      info_page_src = ke2dbg_pageinfo_get((unsigned int)i, NULL, NULL, &level )& 0xFFFFF000;  
      if (info_page_src == ad)
	{
	  ke2dbg_sprintf(debug, "%.8X", i);
	  ke2dbg_output(debug);
	}
    }

  return 0;
}

/**
 * @param ptr
 * @param v
 */
void poked1(unsigned int ptr, unsigned int v)
{
  unsigned int info_page_src;
  unsigned int level;
  unsigned char* ptr_flying = (unsigned char*)(((unsigned int) &flying_page1[0x1000]) & 0xFFFFF000);
  unsigned int ptr_hi = ptr & 0xFFFFF000;
  unsigned int ptr_lo = ptr & 0x00000FFF;
  //unsigned char debug[80];
  //unsigned int toto;
  ptr_lo &= 0xFFFFFFFC;

#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t"
	);
#endif

  info_page_src = ke2dbg_pageinfo_get((unsigned int)ptr_flying, NULL, NULL, &level );  

  if (info_page_src & 0x80)
    {
      Out_Debug_String("bad\n");
      return;      
    }

  //printk("%X %X %X %X\n", ptr_flying, ptr_hi + (info_page_src & 0x00000FFF), &ptr_flying[ptr_lo], level);
  ke2dbg_wpbit_reset();

  ke2dbg_pageinfo_set((unsigned int)ptr_flying, ptr_hi + (info_page_src & 0x00000EFF), level);

#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t"	);
#endif

  *((unsigned int*)(&ptr_flying[ptr_lo])) =v;

  ke2dbg_pageinfo_set((unsigned int)ptr_flying, info_page_src, level);

#if defined(__GNUC__)
  _asm_("\t"
	"pushl %eax\n\t"
	"movl %cr3, %eax\n\t"
	"movl %eax, %cr3\n"
	"popl %eax\n\t");
#endif

  ke2dbg_wpbit_set();
}
