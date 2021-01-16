

sdl_flags := $(shell pkg-config --cflags sdl2)
sdl_libs  := $(shell pkg-config --libs sdl2)

all:
	clang game.c glad.c helpers.c linalg.c \
		$(sdl_flags) $(sdl_libs) \
		-ggdb -o main \
		-Iinclude -lGL -lglfw -lm -ldl
o:
	@rm -f main
	gcc game.c glad.c helpers.c linalg.c \
		-O2 -o main \
		-Iinclude -lGL -lglfw -lm -ldl \
		-march=native -mavx
	@./main
s:
	@rm -f main
	clang game.c glad.c helpers.c linalg.c \
		-O1 -g -o main \
		-fsanitize=undefined,memory \
		-Iinclude -lm -ldl -lX11 -lGL -lglfw -SDL2\
		-march=native -mavx
	ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer ./main


run:
	./main

.PHONY: clean
clean:
	rm -f main	
