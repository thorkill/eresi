#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>


#define  BEGIN_PACKET   "$"
#define  END_PACKET     "#"
#define  QSUPPORTED     "qSupported"

void     gdbwrap_query_supported(const int * fd);
