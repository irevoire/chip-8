# src makefile

BASE   := ..
COMMON := ${BASE}/common.mk
SRC    := $(wildcard *.c)
HDR    := $(wildcard ${BASE}/include/*.h)
OBJDIR := ${BASE}/obj/src
OBJ    := $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))

include ${COMMON}

CFLAGS += -I${BASE}/include

all: ${OBJ}

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo "[*] Cleaning"
	@rm -rf ${LIB} ${OBJ} ${COVDIR}
