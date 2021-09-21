# Vikman
# April 18, 2021

# Targets:
#   all (default)Build tests.
#   check                   Run tests.
#   clean                   Clean tests.
#
# Modes:
#   MODE=release            Optimized build for release (default).
#   MODE=debug              Build with debugging symbols.
#
# Sanitizers:
#   SANITIZE=<sanitizer>    Enable sanitizer.

SOURCES = $(wildcard *.c)
TARGET = $(SOURCES:.c=)

CC = clang
CFLAGS = -Wall -Wextra -pipe -pthread -I../include
LDFLAGS = -pthread -L../lib
LIBS = -lbqueue

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

.PHONY: all
all: $(TARGET)

test_%: test_%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

test_fuzzer: test_fuzzer.c
	$(CC) $(CFLAGS) $(LDFLAGS) -fsanitize=fuzzer -o $@ $^ $(LIBS)

.PHONY: check
check:
	@$(foreach T, $(TARGET),./$(T))

.PHONY: clean
clean:
	$(RM) $(TARGET)
