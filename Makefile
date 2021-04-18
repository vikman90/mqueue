# Vikman
# April 18, 2021

# Targets:
#   all (default)           Build library.
#   install                 Install library.
#   uninstall               Uninstall library.
#   clean                   Clean full project.
#   check                   Build and run tests.
#
# Modes:
#   MODE=release            Optimized build for release (default).
#   MODE=debug              Build with debugging symbols.
#
# Sanitizers:
#   SANITIZE=<sanitizer>    Enable sanitizer.
#
# Options:
#    SHARED=yes             Build shared library instead of static library.

NAME_SHARED = libmqueue.so
NAME_STATIC = libmqueue.a

SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(wildcard include/*.h)

CFLAGS = -Wall -Wextra -pipe -pthread -I./include
LDFLAGS = -pthread

prefix = /usr/local
includedir = $(prefix)/include
libdir = $(prefix)/lib

ifeq ($(MODE),debug)
	CFLAGS += -g -O1
ifdef SANITIZE
	CFLAGS += -fno-omit-frame-pointer -fsanitize=$(SANITIZE)
	LDFLAGS += -fsanitize=$(SANITIZE)
endif
else
	CFLAGS += -O2 -fdata-sections -ffunction-sections
	LDFLAGS += -s -Wl,--gc-sections
	DEFINES += -DNDEBUG
endif

CFLAGS += $(DEFINES)

ifeq ($(SHARED),yes)
	TARGET = lib/$(NAME_SHARED)
	CFLAGS += -fPIC
else
	TARGET = lib/$(NAME_STATIC)
endif

.PHONY: all
all: $(TARGET)

lib/$(NAME_SHARED): $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

lib/$(NAME_STATIC): $(OBJECTS)
	$(AR) rc $@ $^

$(OBJECTS): $(HEADERS)

.PHONY: install
install:
	install $(HEADERS) $(includedir)
	install $(TARGET) $(libdir)

.PHONY: uninstall
uninstall:
	$(RM) $(includedir)/mqueue.h
	$(RM) $(libdir)/$(NAME_SHARED) $(libdir)/$(NAME_STATIC)

.PHONY: clean
clean:
	$(RM) lib/$(NAME_SHARED) lib/$(NAME_STATIC) $(OBJECTS)
	make -C test clean

.PHONY: check
check:
	make -C test
	make -C test check
