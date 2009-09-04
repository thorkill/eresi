##
## Makefile for libedfmt testsuite in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

all			: all32 all64

all32			: 
			$(MAKE) -C load all32
			$(MAKE) -C srcline all32
			$(MAKE) -C stackframes all32

all64			: 
			$(MAKE) -C load all64
			$(MAKE) -C srcline all64
			$(MAKE) -C stackframes all64

clean		       :
			$(MAKE) -C load clean
			$(MAKE) -C srcline clean
			$(MAKE) -C stackframes clean

fclean		       : clean
			$(MAKE) -C load fclean
			$(MAKE) -C srcline fclean
			$(MAKE) -C stackframes fclean
