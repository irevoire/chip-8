CFLAGS ?=
CFLAGS += $(shell sdl2-config --cflags)

LDFLAGS ?=
LDFLAGS += $(shell sdl2-config --libs)
