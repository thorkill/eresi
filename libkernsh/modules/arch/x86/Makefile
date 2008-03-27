OS	:= $(shell uname -s | sed -e s/Linux*/Linux/)

ifeq ($(OS),Linux)
all:
	$(MAKE) -C linux 

clean:
	$(MAKE) -C linux clean

fclean:
	$(MAKE) -C linux fclean
endif
