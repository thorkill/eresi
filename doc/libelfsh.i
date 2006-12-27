%module libelfsh

%include "typemaps.i"
%apply int *INOUT { int *offset };
%apply int *INOUT { int *num };
%apply int *INOUT { int *off };



%{
#include <libelfsh.h>

/* "Typecasts" to make void* references usable in Python: */
Elf32_Rel* toElf32_Rel( void *input) {
  return input;
}

Elf32_Rela* toElf32_Rela( void *input) {
  return input;
}

Elf32_Sym* toElf32_Sym( void *input) {
  printf( "toElf32_Sym(): %x \n", input);
  return input;
}

Elf32_Dyn* toElf32_Dyn( void *input) {
  return input;
}

elfsh_Sym* to_elfsh_Sym( void *input) {
  return input;
}
%}

//typedef u_long uint32_t;
//#define elfsh_Sym Elf32_Sym

%rename(__secthash) secthash;
%rename(secthash) getsecthash;

%include <libelfsh/libelfsh-compat.h>
%include <libelfsh.h>


/* "Typecasts" to make void* references usable in Python: */
Elf32_Rel* toElf32_Rel( void *input);
Elf32_Rela* toElf32_Rela( void *input);
Elf32_Sym* toElf32_Sym( void *input);
Elf32_Dyn* toElf32_Dyn( void *input);
elfsh_Sym* to_elfsh_Sym( void *input);

%extend elfshobj_t {
  elfshsect_t *getsecthash( int index) {
    return (self->secthash)[index];
  }
}
  
%extend elfshsect_t {
  char *getdata() {
    return (self->data);
  }


  void *getdata(int index) {
    int sh_entsize;
    if(self->shdr->sh_entsize == 0) {
      sh_entsize = 1;
    } else {
      sh_entsize = self->shdr->sh_entsize;
    }
    if( (self->data != 0) && ((sh_entsize * index) < self->shdr->sh_size)) {
      return (self->data+sh_entsize*index);
    } else {
      return(0);
    } 
  }

  char *getaltdata() {
    return (self->terdata);
  }

  char *getterdata() {
    return (self->altdata);
  }
    
}


/*
11:06 <@Kasperle> but do you need to do that with elfshsect_t.data?
11:08 <@heroine> hmm .. i don't understand what you ask for :)
11:08 <@thorkill> if you want to use data as universal container then yes
11:08 <@thorkill> becouse .data can be verething
11:09 <@Kasperle> i'm working on python mappings for libelfsh with swig. and that has some trouble with void*, because it can't map that to useful python
                  types
11:09 <@thorkill> it depends on what a section represents ;-) afaik
11:10 <@thorkill> if you want to put controlflow binarytree into .data so you have to map it into binarytree
11:11 <@thorkill> if you are plaing with strings then map it into char *
11:11 <@novasilvr> Kasperle, the void * pointer is usually casted to something else before using, since that buffer can contain various kind of array of
                   different element type depending on the section type
11:12 <@thorkill> void represends a universal data type, which can be mapped into another type it depends on context ;-)
11:13 <@Kasperle> i thought maybe libelfsh uses it in one way only
11:13 <@novasilvr> no
11:14 <@novasilvr> can be array of char (casted to char*), array of elfsh_Sym*, array of everything that may be contained within a section
11:14 <@Kasperle> ok. is its size stored somewhere?
11:15 <@novasilvr> sect->shdr->sh_size
11:15 <@thorkill> ptr size ;-)
11:15 <@novasilvr> Kasperle, size of what ?
11:15 <@novasilvr> of the elem ?
11:15 <@Kasperle> of the structure that data points to
11:15 <@novasilvr> sect->shdr->sh_entsz may do the job
11:15 <@Kasperle> ok
11:16 <@novasilvr> sect beeing the elfshsect_t that contains the *data
11:17 <@novasilvr> Kasperle, sect->shdr->sh_entsz is the size of 1 element of the section
11:17 <@novasilvr> the section size is in sect->shdr->sh_size
11:17 <@novasilvr> with those 2 info you can compute the number of elemes
11:17 <@novasilvr> for that section
11:18 <@Kasperle> ok
11:18 <@novasilvr> if sect->shdr->sh_entsz == 0
11:18 <@novasilvr> consider it = 1
11:18 <@novasilvr> meaning the section is raw data .. or ascii ..
*/