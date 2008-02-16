/**
 * @file libe2dbg/kernel/buffering.c
 *
 *
 */
#include "ke2dbg.h"


/**
 * Get IDT address 
 */
unsigned int* ke2dbg_idt_getbase()  
{
  _asm_("sidt idt_");

  idt_[0]=(idt_[0]>>16);
  idt_[1]=(idt_[1]<<16);
  idt_[0]=(idt_[0]|idt_[1]);

  return (unsigned int*)idt_[0];
}


/**
 * Get the linear address for GDT 
 */
unsigned int* ke2dbg_gdt_getbase()  
{
  _asm_("sgdt gdt_");

  gdt_[0]=(gdt_[0]>>16);
  gdt_[1]=(gdt_[1]<<16);
  gdt_[0]=(gdt_[0]|gdt_[1]);

  return (unsigned int*)gdt_[0];
};

/**
 *
 */
unsigned int ke2dbg_ldt_getbase(void)  
{
  _asm_("sldt ldt_\n");

  return ldt_[0];
}

/**
 *
 */
unsigned int ke2dbg_virt2lin_convert(unsigned int segment, unsigned int adresse)
{
  unsigned int index;
  unsigned int *  dt_base;
  unsigned int tmp_ldt;
  unsigned int data1, data2;
  unsigned int adresse_base;

  dt_base = ke2dbg_gdt_getbase();
  if (segment & 0x4)
    {
      tmp_ldt = ke2dbg_ldt_getbase();

      tmp_ldt = tmp_ldt>>3;

      data1 = dt_base[tmp_ldt*2];
      data2 = dt_base[tmp_ldt*2+1];

      adresse_base= (data1 & 0xffff0000)>>16;
      adresse_base+=(data2 & 0x000000ff)<<16;
      adresse_base+=(data2 & 0xff000000);
      dt_base = (unsigned int*)adresse_base;
    }

  index = segment >>3;
  data1 = dt_base[index*2];
  data2 = dt_base[index*2+1];

  adresse_base= (data1 & 0xffff0000)>>16;
  adresse_base+=(data2 & 0x000000ff)<<16;
  adresse_base+=(data2 & 0xff000000);
  //if (segment & 4)
  //  printk("local! %X \n", adresse_base);
  return  adresse+adresse_base;


}


/** 
 * Print content of IDT 
 * @param adresse_idt
 */
void  ke2dbg_idt_print(unsigned long *adresse_idt)
{
  unsigned int offset;
  unsigned int selecteur;
  int x;
  char tmp[2048];

  ke2dbg_logbuf_insert("[_    idt list    _]");
  ke2dbg_logbuf_insert("--------------------");

  for(x=0;x<=0x80;)
    {
      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      ke2dbg_sprintf(debug__, "%.2X %.8X %.8X", x, selecteur, offset);
      x++;

      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      ke2dbg_sprintf(tmp, "%s ³ %.2X %.8X %.8X", debug__, x, selecteur, offset);
      x++;

      offset=((unsigned long) (*adresse_idt) & 0xffff);
      selecteur=((unsigned long) (*adresse_idt) & 0xffff0000)>>16;
      adresse_idt++;
      offset=offset+((unsigned long) (*adresse_idt) & 0xffff0000);
      adresse_idt++;
      ke2dbg_sprintf(debug__, "%s ³ %.2X %.8X %.8X", tmp, x, selecteur, offset);
      x++;

      ke2dbg_logbuf_insert(debug__);

    }
}

/** 
 * modifie l'entrée de l'idt par l'arg de la
 * fonction passée en parametre 
 * renvoie l'ancien pointeur de l'entrée           
 * @param newhandler
 * @param idtentry
 *
 *
 */
unsigned long ke2dbg_idt_modify(unsigned int newhandle,unsigned int *idtentry)
{
  unsigned long aux;


  aux=((unsigned long) (*idtentry) & 0xffff);
  *idtentry=(((unsigned long) (*idtentry) & 0xffff0000)|(newhandle & 0xffff));
  idtentry++;
  aux=aux+((unsigned long) (*idtentry) & 0xffff0000);
  *idtentry=(((unsigned long) (*idtentry) & 0xffff)|(newhandle & 0xffff0000)); 
  return aux;

}

/**
 * @param idtentry
 */
unsigned long ke2dbg_idt_readent(unsigned int *idtentry)
{
  unsigned long aux;

  aux=((unsigned long) (*idtentry) & 0xffff);
  idtentry++;
  aux=aux+((unsigned long) (*idtentry) & 0xffff0000);
  return aux;
}
