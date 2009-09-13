/**
* @file libe2dbg/kernel/kern-bp.c
 */
#include "ke2dbg.h"

Bp_struct list_breakpoints[NUM_BREAKPOINTS]={
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
};           

Bp_struct list_hw_breakpoints[NUM_HW_BREAKPOINTS]={
  {0,0,0,0,0}, {0,0,0,0,0},
  {0,0,0,0,0}, {0,0,0,0,0},
};           

int unmapped_bp_to_shoot=-1;
unsigned int debug_reg_value;
unsigned int iop;




/** 
 * insertion d'un breakpoint:                      
 * /!\ la page de memoire doit etre deja bonne    
 * sauvegarde l'ancien octet ecrasé par le break   
 * point, et ajoute dans la liste des bp.
 */          
int ke2dbg_insert_bp(unsigned int cs, unsigned int dir, unsigned int attrib)
{
  int x=0;
  unsigned int info_page_src;
  unsigned int level;
  unsigned int padresse;
  unsigned char *ptr = (unsigned char*)ke2dbg_virt2lin_convert(cs, dir);

  if (!ptr)
    {
      ke2dbg_output("All but this address man");
      return -1;
    }

  if (!ke2dbg_page_exist((unsigned int) ptr))
    {
      ke2dbg_output("page doesn't exist, man");
      return -1;
    }

  info_page_src = ke2dbg_pageinfo_get((unsigned int)ptr, NULL, NULL, &level );
  padresse = (info_page_src & 0xfffff000) + ((unsigned int)ptr & 0xfff);


  for (x=0;x<NUM_BREAKPOINTS;x++)
    {
      if (list_breakpoints[x].address==0)
	break;
    }

  if (x<NUM_BREAKPOINTS)
    {
      list_breakpoints[x].address=dir;
      list_breakpoints[x].value=*ptr;
      list_breakpoints[x].paddress=padresse;
      list_breakpoints[x].attrib=attrib;

      write_save_dirty( ptr, 0xcc);
      return x;
    }
  else 
    {
      ke2dbg_output("too many breakpoint man");
      return -1;
    }	
}

/**
 * renvoie si l'adresse donnée est un break point 
 * le numéro du breakpoint si oui                  
 * -1 sinon                                        
 */                                               
int ke2dbg_is_breakpoint(unsigned char *ptr)
{
  int i;
  unsigned int padresse;


  if (!ptr)
    return -1;

  for (i=0;i<NUM_BREAKPOINTS;i++)
    if (list_breakpoints[i].address==(unsigned long) ptr) break;
              
  if (i<NUM_BREAKPOINTS)
    return i;

	
  padresse = ke2dbg_linear2physical((unsigned int)ptr);
	
  for (i=0;i<NUM_BREAKPOINTS;i++)
    {
      if (!list_breakpoints[i].address) continue;
      if (list_breakpoints[i].paddress==padresse) break;
    }
	
  if (i<NUM_BREAKPOINTS)
    return i;
	
  return -1;
}


/**
 * enleve un break point:
 * remet l'octet ecrasé par le CC, et enleve
 * le break point de la liste
 * /!\ la page de l'adresse du bp doit etre en mem
 */
int ke2dbg_erase_breakpoint(unsigned int bp)
{
  unsigned char *pbp;
  unsigned int padresse;

  if ((bp>= HW_MAGIC_VALUE) && (bp < HW_MAGIC_VALUE + NUM_HW_BREAKPOINTS))
    {
      bp-=HW_MAGIC_VALUE;

      switch(bp)
	{
	case 0:
	  ke2dbg_dr0_set(0); break;
	case 1:
	  ke2dbg_dr1_set(0); break;
	case 2:
	  ke2dbg_dr2_set(0); break;
	case 3:
	  ke2dbg_dr3_set(0); break;
	default:
	  ke2dbg_output("but man, what are u doin' there?");
	  break;
	}

      list_hw_breakpoints[bp].address=0;
      return bp;
    }

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (!list_breakpoints[bp].address)
    return -1;


  if (!ke2dbg_page_exist(list_breakpoints[bp].address))
    {
      ke2dbg_output("yup! bp is not in ram anymore!! system may be unstable");
      return -1;
    }

  padresse = ke2dbg_linear2physical((unsigned int)list_breakpoints[bp].address);
	
  if (padresse !=list_breakpoints[bp].paddress)
    {
      if ((unmapped_bp_to_shoot!=bp))
	{
	  ke2dbg_output("lin@ doesnt match phys@. fall back in int del");
	  unmapped_bp_to_shoot = bp;
	  return -1;
	}
    }
	
  pbp= (unsigned char *) list_breakpoints[bp].address;

  write_save_dirty( pbp, list_breakpoints[bp].value);
	
  list_breakpoints[bp].address=0;
  list_breakpoints[bp].value=0;  
  return bp;	
}



/**
 * remet le bon octet ecrasé par le bp, sans       
 * l'enlever de la liste des bp                    
 * /!\ la page doit etre en mem                   
 *                                                
 */
int ke2dbg_pass_breakpoint(unsigned int bp)
{
  unsigned char *pbp;

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (list_breakpoints[bp].address)
    {
      pbp= (unsigned char *) list_breakpoints[bp].address;
      write_save_dirty( pbp, list_breakpoints[bp].value);
      return bp;
    }
  else
    {
      return -1;
    }

}

/** 
 * remet un CC sur l'adresse du bp spécifié        
 * /!\ la page doit etre en mem                   
 */
int ke2dbg_spawn_breakpoint(unsigned int bp)
{
  unsigned char *pbp;

  if (bp>NUM_BREAKPOINTS)
    return -1;

  if (list_breakpoints[bp].address)
    {
      pbp= (unsigned char *) list_breakpoints[bp].address;
      //write_save_dirty( pbp, list_breakpoints[bp].value);
      write_save_dirty( pbp, 0xCC);
      return bp;
    }
  else
    {
      return -1;
    }
}


/** 
 * affiche la liste des breakpoint                 
 * (dans la fenetre de log)                        
 *                                                
 *                                                
 */
void ke2dbg_display_breakpoints()
{
  unsigned int i=0;
	
  ke2dbg_output("[_Break point List_]");

  for (i=0;i<NUM_BREAKPOINTS;i++)
    {
      if (list_breakpoints[i].address)
	{
	  ke2dbg_sprintf(debug__, "%.4X %.8X", i, list_breakpoints[i].address);
	  ke2dbg_output(debug__);
	}
    }


  for (i=0;i<4;i++)
    {
      if (list_hw_breakpoints[i].address)
	{
	  ke2dbg_sprintf(debug__, "%.4X %.8X", i+HW_MAGIC_VALUE, list_hw_breakpoints[i].address);
	  ke2dbg_output(debug__);
	}
    }
}



/**
 * si l'octet de l'adresse specifie a été remplace 
 * par un bp, renvoie l'ancien octet               
 * renvoie l'octet de l'adresse sinon              
 */
unsigned char ke2dbg_true_byte_bp(unsigned char *ptr)
{
  int bp_num;

  if ((bp_num=ke2dbg_is_breakpoint(ptr))!=-1)
    return list_breakpoints[bp_num].value;
	
  return *ptr;
}


#if defined(__GNUC__)
#define get_val(reg) _asm_( "\t" \
	"push %eax\n\t" \
	"movl %" reg ", %eax\n\t" \
	"movl %eax, debug_reg_value\n\t" \
	"popl %eax\n\t" \
	)
#elif defined(_MSC_VER)
#define get_val(reg)\
{\
 __asm{push eax}\
 __asm{mov eax, reg}\
 __asm{mov debug_reg_value, eax}\
 __asm{pop eax}\
}
#endif

#if defined(__GNUC__)
#define set_val(reg) _asm_( "\t" \
	"push %eax\n\t" \
	"movl debug_reg_value, %eax\n\t" \
	"movl %eax, %" reg "\n\t" \
	"popl %eax\n\t" \
	)
#elif defined(_MSC_VER)
#define set_val(reg) \
{\
__asm{push eax}\
__asm{mov eax, debug_reg_value}\
__asm{mov reg, eax}\
__asm{pop eax}\
}
#endif

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr0_get()
{
#if defined(__GNUC__)
  get_val("dr0");
#elif defined(_MSC_VER)
  get_val(dr0);
#endif

  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr1_get()
{
#if defined(__GNUC__)
  get_val("dr1");
#elif defined(_MSC_VER)
  get_val(dr1);
#endif

  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr2_get()
{
#if defined(__GNUC__)
  get_val("dr2");
#elif defined(_MSC_VER)
  get_val(dr2);
#endif

  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr3_get()
{
#if defined(__GNUC__)
  get_val("dr3");
#elif defined(_MSC_VER)
  get_val(dr3);
#endif

  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr4_get()
{
  /*
#if defined(__GNUC__)
  get_val("dr4");
#elif defined(_MSC_VER)
  get_val(dr4);
#endif
  */
  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr5_get()
{
  /*
#if defined(__GNUC__)
  get_val("dr5");
#elif defined(_MSC_VER)
  get_val(dr5);
#endif
  */
  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr6_get()
{
#if defined(__GNUC__)
  get_val("dr6");
#elif defined(_MSC_VER)
  get_val(dr6);
#endif

  return debug_reg_value;
}

/**
 * @return Return register <COMPLETE>
 */
unsigned int ke2dbg_dr7_get()
{
#if defined(__GNUC__)
  get_val("dr7");
#elif defined(_MSC_VER)
  get_val(dr7);
#endif

  return debug_reg_value;
}


/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr0_set(unsigned int value)
{
  debug_reg_value = value;


#if defined(__GNUC__)
  set_val("dr0");
#elif defined(_MSC_VER)
  set_val(dr0);
#endif

}


/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr1_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr1");
#elif defined(_MSC_VER)
  set_val(dr1);
#endif

}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr2_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr2");
#elif defined(_MSC_VER)
  set_val(dr2);
#endif

}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr3_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr3");
#elif defined(_MSC_VER)
  set_val(dr3);
#endif

}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr4_set(unsigned int value)
{
  debug_reg_value = value;
  /*
#if defined(__GNUC__)
  set_val("dr4");
#elif defined(_MSC_VER)
  set_val(dr4);
#endif
  */
}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr5_set(unsigned int value)
{
  debug_reg_value = value;
  /*
#if defined(__GNUC__)
  set_val("dr5");
#elif defined(_MSC_VER)
  set_val(dr5);
#endif
  */
}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr6_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr6");
#elif defined(_MSC_VER)
  set_val(dr6);
#endif

}

/**
 * Set register <COMPLETE> to value
 * @param value
 */
void ke2dbg_dr7_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  set_val("dr7");
#elif defined(_MSC_VER)
  set_val(dr7);
#endif

}

/**
 * Return content of register cr4
 * @return Value of register cr4
 */
unsigned int ke2dbg_cr4_get()
{
#if defined(__GNUC__)
  //get_val("cr4");
#elif defined(_MSC_VER)
  //get_val(cr4);
#endif

  return debug_reg_value;
}

/**
 * Set cr4 to value
 * @param value New value of cr4
 */
void ke2dbg_cr4_set(unsigned int value)
{
  debug_reg_value = value;

#if defined(__GNUC__)
  //set_val("cr4");
#elif defined(_MSC_VER)
  //set_val(cr4);
#endif

}

/**
 *
 */
void ke2dbg_hwbp_reset()
{
  int x;

  for (x=0;x<NUM_HW_BREAKPOINTS;x++)
    list_hw_breakpoints[x].address=0;

  ke2dbg_dr6_set(0x0);
  ke2dbg_dr7_set(0x0);

  ke2dbg_dr0_set(0x0);
  ke2dbg_dr1_set(0x0);
  ke2dbg_dr2_set(0x0);
  ke2dbg_dr3_set(0x0);


}

/**
 * @param ad
 * @return
 */
int ke2dbg_is_hw_breakpoint(unsigned char* ad)
{
  unsigned int x;
  for (x=0;x<NUM_HW_BREAKPOINTS;x++)
    if(list_hw_breakpoints[x].address==(unsigned int )ad)
      return 1;

  return 0;

}

/**
 * @param ad
 */
void ke2dbg_hwbp_set(unsigned int ad)
{
  unsigned int mask_ge_le = 0x300;
  int num_debug_reg;
  unsigned int mask_current_bp;
  unsigned int dr7_val;

  for (num_debug_reg=0;num_debug_reg<NUM_HW_BREAKPOINTS;num_debug_reg++)
    if (!list_hw_breakpoints[num_debug_reg].address)
      {
	break;
      }


  

  if ((num_debug_reg <0 )||(num_debug_reg >3))
    {
      ke2dbg_output("Oao, only 4 hw bp on 86, man");
      return ;
    }

  mask_current_bp = 0x3<<(num_debug_reg*2);
  /*only exec*/
  mask_current_bp &= ~(((0x0<<(num_debug_reg*2)) << 16));

  /*TEST TEST*/


  dr7_val = ke2dbg_dr7_get();


  dr7_val |= (mask_ge_le | mask_current_bp);


  ke2dbg_dr7_set(dr7_val);
  switch(num_debug_reg)
    {
    case 0:
      ke2dbg_dr0_set(ad); break;
    case 1:
      ke2dbg_dr1_set(ad); break;
    case 2:
      ke2dbg_dr2_set(ad); break;
    case 3:
      ke2dbg_dr3_set(ad); break;
    default:
      ke2dbg_output("but man, what are u doin' there?");
      break;
    }

  list_hw_breakpoints[num_debug_reg].address = ad;


}


/**
 * @return
 */
int ke2dbg_hwbp_check()
{
  unsigned int dr6_val;

  dr6_val = ke2dbg_dr6_get();
  return dr6_val & 0xf;
}

/**
 * @param i
 */
void ke2dbg_hwbp_enable(int i)
{
  unsigned int mask= 0x3 << (2*i);
  unsigned int dr7_val;

  dr7_val = ke2dbg_dr7_get();
  dr7_val|= mask;
  ke2dbg_dr7_set(dr7_val);

}

/**
 * @param i
 */
void ke2dbg_hwbp_disable(int i)
{
  unsigned int mask=~(0x3 << (2*i));
  unsigned int dr7_val;

  dr7_val = ke2dbg_dr7_get();
  dr7_val&= mask;
  ke2dbg_dr7_set(dr7_val);

}

/**
 * @param id_hw_bp
 */
void ke2dbg_idhwbp_enable(int id_hw_bp)
{
  int i;

  for (i=0;i<4;i++)
    {
      if (id_hw_bp & (1<<i))
	ke2dbg_hwbp_enable(i);
    }
}

/**
 * @param id_hw_bp
 */
void ke2dbg_idhwbp_disable(int id_hw_bp)
{
  int i;

  for (i=0;i<4;i++)
    {
      if (id_hw_bp & (1<<i))
	ke2dbg_hwbp_disable(i);
    }
}

