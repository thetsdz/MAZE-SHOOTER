ifeq ($(OS),Windows_NT)
MAKEFILE = Makefile.windows
else
MAKEFILE = Makefile.linux
endif

all:
	$(MAKE) -f $(MAKEFILE) all

clean:
	$(MAKE) -f $(MAKEFILE) clean

fclean:
	$(MAKE) -f $(MAKEFILE) fclean

re:
	$(MAKE) -f $(MAKEFILE) re

info:
	$(MAKE) -f $(MAKEFILE) info

run:
	$(MAKE) -f $(MAKEFILE) run
%:
	$(MAKE) -f $(MAKEFILE) $@

.PHONY: all run clean fclean re debug release info