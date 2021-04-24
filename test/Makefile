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

SOURCES = $(wildcard *.cpp)
TARGET = $(SOURCES:.cpp=)

CXX = clang++
CXXFLAGS = -Wall -Wextra -pipe -I../include
LDFLAGS = -L../lib
LIBS = -lmqueue -lpthread

ifeq ($(MODE),debug)
	CXXFLAGS += -g -O1
ifdef SANITIZE
	CXXFLAGS += -fno-omit-frame-pointer -fsanitize=$(SANITIZE)
	LDFLAGS += -fsanitize=$(SANITIZE)
endif
else
	CXXFLAGS += -O2 -fdata-sections -ffunction-sections
	LDFLAGS += -s -Wl,--gc-sections
	DEFINES += -DNDEBUG
endif

CXXFLAGS += $(DEFINES)

.PHONY: all
all: $(TARGET)

test_%: test_%.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

test_fuzzer: test_fuzzer.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -fsanitize=fuzzer -o $@ $^ $(LIBS)

.PHONY: check
check:
	@$(foreach T, $(TARGET),./$(T))

.PHONY: clean
clean:
	$(RM) $(TARGET)
