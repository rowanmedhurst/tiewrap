CC = gcc
SDL_INC = `sdl2-config --cflags`
SDL_LIB = `sdl2-config --libs`
DUKTAPE_INC = -Iduktape/src -Iduktape/extras/console -Iduktape/extras/module-node
DUKTAPE_SRC = duktape/src/duktape.c duktape/extras/console/duk_console.c duktape/extras/module-node/duk_module_node.c
#GIT_DESCRIBE := $(shell git describe --always --dirty)
CFLAGS = -Wall -c -std=c99 -I. $(SDL_INC) $(DUKTAPE_INC)
LDFLAGS = $(SDL_LIB)
SRCS = $(DUKTAPE_SRC) $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)
EXE = tiewrap

all: $(OBJS) $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)

test:
	@echo $(OBJS)