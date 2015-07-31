CC=clang
CFLAGS=-g -Wall -O3 -Isrc -I/usr/include/SDL2 -D_REENTRANT
LDLIBS=-lm -lSDL2 -lpthread

SOURCES=$(wildcard src/*.c)
HEADERS=$(wildcard src/*.h)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

BIN_SOURCES=$(wildcard bin/*.c)
BINS=$(basename $(BIN_SOURCES))

TEST_SOURCES=$(wildcard tests/*.c)
TESTS=$(basename $(TEST_SOURCES))

default: tags $(BINS) $(TESTS)

$(BINS): %: %.c $(OBJECTS)

CFLAGS+=-Itests
$(TESTS): %: %.c $(OBJECTS)

$(OBJECTS): %.o: %.c $(HEADERS)

tags: $(SOURCES) $(HEADERS) $(BIN_SOURCES) $(TEST_SOURCES)
	ctags $^

clean:
	rm -f $(OBJECTS) $(BINS) $(TESTS) tags

.PHONY: clean
