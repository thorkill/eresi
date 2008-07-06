##
## Makefile for librevm in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

include ../config.h

all			: all32 all64

all32			: 
			$(MAKE) -C debugging all32
			$(MAKE) -C elf all32
			$(MAKE) -C tracing all32
			$(MAKE) -C static_analysis all32
			$(MAKE) -C static_instrumentation all32

all64			: 
			$(MAKE) -C debugging all64
			$(MAKE) -C elf all64
			$(MAKE) -C tracing all64
			$(MAKE) -C static_analysis all64
			$(MAKE) -C static_instrumentation all64

clean		       :
			$(MAKE) -C debugging clean
			$(MAKE) -C elf clean
			$(MAKE) -C tracing clean
			$(MAKE) -C static_analysis clean
			$(MAKE) -C static_instrumentation clean

fclean		       : clean
		   	$(MAKE) -C debugging fclean
			$(MAKE) -C elf fclean
			$(MAKE) -C tracing fclean
			$(MAKE) -C static_analysis fclean
			$(MAKE) -C static_instrumentation fclean
