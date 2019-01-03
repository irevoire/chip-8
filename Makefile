# Main makefile

SUBDIR := src main

OBJDIR := obj
BINDIR := bin

export # allow all variables to be inclued in the sub Makefile

.PHONY: clean test all term sdl

all:
	@for dir in ${SUBDIR} ; do \
		echo "[*] Building subdir $$dir" ; \
		$(MAKE) -C $$dir || exit ;\
	done

term: GFX=TERM
term: all

ncurses: GFX=NCURSES
ncurses: all

sdl: GFX=SDL
sdl: all


clean:
	@echo "[*] Cleaning"
	@rm -rf ${OBJDIR} ${BINDIR}
