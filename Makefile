sdl_flags := $(shell pkg-config --cflags sdl2)
sdl_libs  := $(shell pkg-config --libs sdl2)

sdl:
	clang glad.c test.c linalg.c helpers.c \
		-lGL -Iinclude -ldl -lm \
	-g -o test \
		$(sdl_flags) $(sdl_libs) \
		-march=native -mavx
	./test
s:
	clang glad.c test.c \
		-lGL -Iinclude/. -ldl \
		-fsanitize=address \
		-O1 -g -o test \
		-march=native -mavx \
	   	 $(sdl_flags) $(sdl_libs)
	ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer ./test

run:
	./main

.PHONY: clean
clean:
	rm -f main	
