%module libelfsh

%include "typemaps.i"
%apply int *INOUT { int *offset };
%apply int *INOUT { int *num };
%apply int *INOUT { int *off };

%{
#include "libelfsh.h"
%}
%rename(__secthash) secthash;
%rename(secthash) getsecthash;

%include "libelfsh.h"

%extend elfshobj_t {
  elfshsect_t *getsecthash( int index) {
    return (self->secthash)[index];
  }
}
