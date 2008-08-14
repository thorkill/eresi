#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  GDBWRAP_BEGIN_PACKET		"$"
#define  GDBWRAP_END_PACKET		"#"
#define  GDBWRAP_QSUPPORTED		"qSupported"
#define  GDBWRAP_DISCONNECT		"k"
#define  GDBWRAP_ERROR			"error"
#define  GDBWRAP_COR_CHECKSUM		"+"
#define  GDBWRAP_BAD_CHECKSUM		"-"
#define  GDBWRAP_PACKET_NO_BEGIN(_x)    (_x + 1)

void     gdbwrap_hello(int fd);
void     gdbwrap_bye(void);


