##
## Makefile for librevm in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

all			: all32 all64

all32			: 
			$(MAKE) -C binaries-multithreads all32
			$(MAKE) -C libedfmt all32
			$(MAKE) -C etrel_memory all32

all64			: 
			$(MAKE) -C binaries-multithreads all64
			$(MAKE) -C libedfmt all64
			$(MAKE) -C etrel_memory all64

clean		       :
			$(MAKE) -C binaries-multithreads clean
			$(MAKE) -C libedfmt clean
			$(MAKE) -C etrel_memory clean

fclean		       : clean
			$(MAKE) -C binaries-multithreads fclean
			$(MAKE) -C libedfmt fclean
			$(MAKE) -C etrel_memory fclean
