# Variable shared by all targets

# Use gcc by default
CC ?= gcc

# Use SDL by default
GFX ?= SDL

# Speed up the build
NPROC_EXISTS := $(shell which nproc)
ifeq ($(NPROC_EXISTS),)
CPUS ?= 1 # probably safer on ARM
else
CPUS := $(shell nproc)
endif
$(info [*] Nb of CPUs: ${CPUS})
MAKEFLAGS += --jobs=$(CPUS)

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CFLAGS ?=
CFLAGS += -g -std=gnu99 $(WARNINGS)

LDFLAGS ?=
