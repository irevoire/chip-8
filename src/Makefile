BASE   := ..
COMMON := ${BASE}/common.mk
include ${COMMON}

SRC    := $(wildcard *.c)
HDR    := $(wildcard ${BASE}/include/*.h)
OBJDIR := ${BASE}/obj/src
OBJ    := $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))

CFLAGS += -I${BASE}/include

.PHONY: clean all gfx

all: ${OBJ} gfx

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	@${CC} -o $@ -c $< ${CFLAGS}

gfx:
	@echo "[*] Building subdir gfx/$$GFX"
	@$(MAKE) -C gfx/${GFX}

clean:
	@echo "[*] Cleaning"
	@rm -rf ${LIB} ${OBJ} ${COVDIR}
