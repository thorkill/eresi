#ifndef KE2DBG_SYSCALLS_H
 #define KE2DBG_SYSCALLS_H

int asmlinkage (*ke2dbg_sys_read)(int fd, void *buf, size_t count);
int asmlinkage (*ke2dbg_sys_write)(int fd, const void *buf, size_t count);

int asmlinkage (*ke2dbg_sys_open)(const char *pathname, int flags, int mode);
int asmlinkage (*ke2dbg_sys_close)(int fd);

#endif
