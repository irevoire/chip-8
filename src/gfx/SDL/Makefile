BASE   := ../../..
COMMON := ${BASE}/common.mk
include ${COMMON}

SRC    := $(wildcard *.c)
HDR    := $(wildcard ${BASE}/include/*.h)
OBJDIR := ${BASE}/obj/${GFX}
OBJ    := $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))

CFLAGS += -I${BASE}/include

include common.mk

all: ${OBJ}

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo "[*] Cleaning"
	@rm -rf ${LIB} ${OBJ} ${COVDIR}
