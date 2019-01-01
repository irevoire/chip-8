# Main makefile

SUBDIR := src main

OBJDIR := obj
BINDIR := bin

export # allow all variables to be inclued in the sub Makefile

all:
	@for dir in ${SUBDIR} ; do \
		echo "[*] Building subdir $$dir" ; \
		$(MAKE) -C $$dir || exit ;\
	done

.PHONY: clean test all

clean:
	@echo "[*] Cleaning"
	@rm -rf ${OBJDIR} ${BINDIR}
