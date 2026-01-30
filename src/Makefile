ifeq ($(OS),Windows_NT)
    MAKEFILE = Makefile.windows
else
    MAKEFILE = Makefile.linux
endif

all:
	make -f $(MAKEFILE) all

run:
	make -f $(MAKEFILE) run

clean:
	make -f $(MAKEFILE) clean

info:
	make -f $(MAKEFILE) info