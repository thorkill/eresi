# Copyright (C) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
# 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
# Free Software Foundation, Inc.

# This file is part of GDB.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

prefix = /usr/local
exec_prefix = ${prefix}

host_alias = 
target_alias = 
program_transform_name = s,x,x,
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
tooldir = $(libdir)/$(target_alias)

datadir = ${prefix}/share
mandir = ${prefix}/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
man9dir = $(mandir)/man9
infodir = ${prefix}/info
htmldir = $(prefix)/html
includedir = ${prefix}/include

SHELL = /bin/sh
EXEEXT = 

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

CC = gcc

# Directory containing source files.  Don't clean up the spacing,
# this exact string is matched for by the "configure" script.
srcdir = .
incdir = include

# It is also possible that you will need to add -I/usr/include/sys to the
# CFLAGS section if your system doesn't have fcntl.h in /usr/include (which 
# is where it should be according to Posix).

# Set this up with gcc if you have gnu ld and the loader will print out
# line numbers for undefinded refs.
#CC-LD=gcc -static
CC-LD=${CC}

# Where is the "include" directory?  Traditionally ../include or ./include
INCLUDE_DIR =  ${srcdir}/../../include
INCLUDE_DEP = $$(INCLUDE_DIR)

# Where is the source dir for the READLINE library?  Traditionally in .. or .
# (For the binary library built from it, we use ${READLINE_DIR}${subdir}.)
READLINE_DIR = ${srcdir}/../readline
READLINE_DEP = $$(READLINE_DIR)

# All the includes used for CFLAGS and for lint.
# -I. for config files.
# -I${srcdir} for our headers.
# -I$(srcdir)/../regformats for regdef.h.
INCLUDE_CFLAGS = -I. -I${srcdir} -I$(incdir)/regformats -I$(INCLUDE_DIR)

# M{H,T}_CFLAGS, if defined, has host- and target-dependent CFLAGS
# from the config/ directory.
GLOBAL_CFLAGS = ${MT_CFLAGS} ${MH_CFLAGS}
#PROFILE_CFLAGS = -pg

WARN_CFLAGS = -Wall

# CFLAGS is specifically reserved for setting from the command line
# when running make.  I.E.  "make CFLAGS=-Wmissing-prototypes".
CFLAGS = -g -O2 -Iinclude

# INTERNAL_CFLAGS is the aggregate of all other *CFLAGS macros.
INTERNAL_CFLAGS =  $(WARN_CFLAGS) ${CFLAGS} ${GLOBAL_CFLAGS} \
	${PROFILE_CFLAGS} ${INCLUDE_CFLAGS} ${BFD_CFLAGS}

# LDFLAGS is specifically reserved for setting from the command line
# when running make.
LDFLAGS = 
INTERNAL_LDFLAGS = $(LDFLAGS) -rdynamic

# All source files that go into linking GDB remote server.

SFILES=	$(srcdir)/gdbreplay.c $(srcdir)/inferiors.c \
	$(srcdir)/mem-break.c $(srcdir)/proc-service.c $(srcdir)/regcache.c \
	$(srcdir)/remote-utils.c $(srcdir)/server.c $(srcdir)/target.c \
	$(srcdir)/thread-db.c $(srcdir)/utils.c \
	$(srcdir)/linux-i386-low.c $(srcdir)/i387-fp.c $(srcdir)/linux-low.c \
	$(srcdir)/linux-mips-low.c $(srcdir)/hostio.c $(srcdir)/hostio-errno.c

DEPFILES = reg-i386-linux.o linux-low.o linux-i386-low.o i387-fp.o hostio-errno.o \
	thread-db.o proc-service.o linux-x86-64-low.o linux-mips-low.o 

SOURCES = $(SFILES)
TAGFILES = $(SOURCES) ${HFILES} ${ALLPARAM} ${POSSLIBS} 

OBS = inferiors.o regcache.o remote-utils.o server.o signals.o target.o \
	utils.o mem-break.o hostio.o $(XML_BUILTIN) $(DEPFILES)

GDBSERVER_LIBS = -lthread_db
XM_CLIBS = 

# XML files to compile in to gdbserver, if any.
XML_DIR = $(srcdir)/../features
XML_TARGET = 
XML_FILES = 
XML_BUILTIN = 

# Prevent Sun make from putting in the machine type.  Setting
# TARGET_ARCH to nothing works for SunOS 3, 4.0, but not for 4.1.
.c.o:
	${CC} -c ${INTERNAL_CFLAGS} $<

all: gdbserver$(EXEEXT) gdbreplay$(EXEEXT)

gdbserver$(EXEEXT): $(OBS) ${ADD_DEPS} ${CDEPS}
	rm -f gdbserver$(EXEEXT)
	${CC-LD} $(INTERNAL_CFLAGS) $(INTERNAL_LDFLAGS) -o gdbserver$(EXEEXT) $(OBS) \
	  $(GDBSERVER_LIBS) $(XM_CLIBS)

gdbreplay$(EXEEXT): gdbreplay.o
	rm -f gdbreplay$(EXEEXT)
	${CC-LD} $(INTERNAL_CFLAGS) $(INTERNAL_LDFLAGS) -o gdbreplay$(EXEEXT) gdbreplay.o \
	  $(XM_CLIBS)

# Put the proper machine-specific files first, so M-. on a machine
# specific routine gets the one for the correct machine.
# The xyzzy stuff below deals with empty DEPFILES
tags: 
	etags *.c include/*.h

clean:
	rm -f *.o ${ADD_FILES} *~
	rm -f gdbserver$(EXEEXT) gdbreplay$(EXEEXT) core make.log
	rm -f reg-i386-linux.c
	rm -f mips-linux.c mips64-linux.c

target.xml: $(XML_TARGET)
	rm -f target.xml
	cp $(XML_TARGET) target.xml

xml-builtin.c: stamp-xml; @true
stamp-xml: $(XML_DIR)/feature_to_c.sh Makefile $(XML_FILES)
	rm -f xml-builtin.tmp
	$(SHELL) $(XML_DIR)/feature_to_c.sh xml-builtin.tmp $(XML_FILES)
	$(SHELL) $(srcdir)/../../move-if-change xml-builtin.tmp xml-builtin.c
	echo stamp > stamp-xml

.PRECIOUS: target.xml xml-builtin.c

# GNU Make has an annoying habit of putting *all* the Makefile variables
# into the environment, unless you include this target as a circumvention.
# Rumor is that this will be fixed (and this target can be removed)
# in GNU Make 4.0.
.NOEXPORT:

# GNU Make 3.63 has a different problem: it keeps tacking command line
# overrides onto the definition of $(MAKE).  This variable setting
# will remove them.
MAKEOVERRIDES=

gdb_proc_service_h = $(incdir)/gdb_proc_service.h
regdat_sh = $(incdir)/regformats/regdat.sh
regdef_h = $(incdir)/regformats/regdef.h
regcache_h = $(incdir)/regcache.h
server_h = $(incdir)/server.h $(regcache_h) $(incdir)/target.h $(incdir)/mem-break.h
terminal_h = $(incdir)/terminal.h

hostio.o: hostio.c $(server_h)
hostio-errno.o: hostio-errno.c $(server_h)
inferiors.o: inferiors.c $(server_h)
mem-break.o: mem-break.c $(server_h)
proc-service.o: proc-service.c $(server_h) $(gdb_proc_service_h)
regcache.o: regcache.c $(server_h) $(regdef_h)
remote-utils.o: remote-utils.c $(terminal.h) $(server_h)
server.o: server.c $(server_h)
target.o: target.c $(server_h)
thread-db.o: thread-db.c $(server_h) $(gdb_proc_service_h)
utils.o: utils.c $(server_h)

signals.o: signals.c $(server_h)
	$(CC) -c $(CPPFLAGS) $(INTERNAL_CFLAGS) $< -DGDBSERVER

i387-fp.o: i387-fp.c $(server_h)

linux_low_h = $(incdir)/linux-low.h

linux-low.o: linux-low.c $(linux_low_h) $(server_h)
	$(CC) -c $(CPPFLAGS) $(INTERNAL_CFLAGS) $< -DUSE_THREAD_DB
linux-i386-low.o: linux-i386-low.c $(linux_low_h) $(server_h) \
	$(gdb_proc_service_h)
linux-mips-low.o: linux-mips-low.c $(linux_low_h) $(server_h) \
	$(gdb_proc_service_h)

linux-x86-64-low.o: linux-x86-64-low.c $(linux_low_h) $(server_h) \
        $(gdb_proc_service_h)

reg-x86-64.o : reg-x86-64.c $(regdef_h)
reg-x86-64.c : $(incdir)/regformats/reg-x86-64.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(incdir)/regformats/reg-x86-64.dat reg-x86-64.c

reg-x86-64-linux.o : reg-x86-64-linux.c $(regdef_h)
reg-x86-64-linux.c : $(srcdir)/../regformats/reg-x86-64-linux.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(srcdir)/../regformats/reg-x86-64-linux.dat reg-x86-64-linux.c

reg-i386.o : reg-i386.c $(regdef_h)
reg-i386.c : $(incdir)/regformats/reg-i386.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(incdir)/regformats/reg-i386.dat reg-i386.c
reg-i386-linux.o : reg-i386-linux.c $(regdef_h)
reg-i386-linux.c : $(incdir)/regformats/reg-i386-linux.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(incdir)/regformats/reg-i386-linux.dat reg-i386-linux.c
mips-linux.o : mips-linux.c $(regdef_h)
mips-linux.c : $(incdir)/regformats/mips-linux.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(incdir)/regformats/mips-linux.dat mips-linux.c
mips64-linux.o : mips64-linux.c $(regdef_h)
mips64-linux.c : $(incdir)/regformats/mips64-linux.dat $(regdat_sh)
	$(SHELL) $(regdat_sh) $(incdir)/regformats/mips64-linux.dat mips64-linux.c



# This is the end of "Makefile.in".
