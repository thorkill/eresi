##
## Makefile for librevm in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

include ../../config.h

all			: all32 all64

all32			: 
			$(MAKE) -C mjollnir all32
			$(MAKE) -C binaries all32

all64			: 
			$(MAKE) -C mjollnir all64
			$(MAKE) -C binaries all64

clean		       :
			$(MAKE) -C mjollnir clean
			$(MAKE) -C binaries clean

fclean		       : clean
		   	$(MAKE) -C mjollnir fclean
			$(MAKE) -C binaries fclean
