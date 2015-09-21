CC = clang
CFLAGS = -g -Wall -O3 -Isrc -Iinclude $(shell pkg-config --cflags glfw3 glew)\
		 -DGLFW_INCLUDE_NONE
LDLIBS = -lm $(shell pkg-config --libs glfw3 glew)

LIB_SOURCES = $(wildcard src/lib/*.c)
LIB_OBJECTS = $(patsubst %.c, %.o, $(LIB_SOURCES))

GAME_SOURCES = $(wildcard src/game/*.c)
GAME_OBJECTS = $(patsubst %.c, %.o, $(GAME_SOURCES))

MAPGEN_SOURCES = $(wildcard src/mapgenerator/*.c)
MAPGEN_OBJECTS = $(patsubst %.c, %.o, $(MAPGEN_SOURCES))

TESTS_SOURCES = $(wildcard tests/*.c)
TESTS = $(basename $(TESTS_SOURCES))

HEADERS = $(wildcard src/**/*.h) $(wildcard src/*.h)
SOURCES = $(wildcard src/**/*.c) $(wildcard src/*.c)
OBJECTS = $(patsubst %.c, %.o,$(SOURCES))
DEPS = $(patsubst %.c, %.d, $(SOURCES))

all: game mapgenerator tags runtests

game: $(LIB_OBJECTS) $(GAME_OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

mapgenerator: $(LIB_OBJECTS) $(MAPGEN_OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

$(TESTS): %: %.c $(LIB_OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD $< -c -o $@

-include $(DEPS)

tags: $(SOURCES) $(HEADERS)
	@ctags $^

.PHONY: runtests
runtests: $(TESTS)
	@echo
	@./runtests.sh

.PHONY: clean
clean:
	@rm -f $(OBJECTS) $(DEPS) $(TESTS) game mapgenerator tags
