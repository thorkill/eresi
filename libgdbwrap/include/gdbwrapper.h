#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  BEGIN_PACKET   "$"
#define  END_PACKET     "#"
#define  QSUPPORTED     "qSupported"
#define  QUIT           "k"
#define  ERROR          "error"

char     *gdbwrap_send_data(const int fd, char * query, char * ret);
