BASE     := ..
COMMON   := ${BASE}/common.mk
include ${COMMON}

SRC      := $(wildcard  *.c)
HDR      := $(wildcard  ${BASE}/include/*.h)
OBJDIR   := ${BASE}/obj/main
OBJ      := $(addprefix ${OBJDIR}/, $(patsubst %.c,%.o,$(SRC)))
VM_OBJ   := ${wildcard  ${BASE}/obj/src/*.o}
GFX_OBJ  := ${wildcard  ${BASE}/obj/${GFX}/*.o}
BINDIR   := ${BASE}/bin
EXE      := ${BINDIR}/main

CFLAGS += -I${BASE}/include

ifeq (${GFX}, SDL)
LDFLAGS += $(shell sdl2-config --libs)
endif

all: ${EXE}

${EXE}: ${OBJ} ${VM_OBJ} ${GFX_OBJ}
	@mkdir -p ${BINDIR}
	@${CC} -o $@ $^ ${LDFLAGS}

${OBJDIR}/%.o: %.c ${HDR} ${COMMON}
	@mkdir -p ${OBJDIR}
	@echo "[*] Building $@"
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo "[*] Cleaning"
	@rm -rf ${LIB} ${OBJ} ${COVDIR}
