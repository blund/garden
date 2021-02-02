#!/bin/sh

CC="clang"
FILES="game.c glad.c linalg.c helpers.c"
SANITIZE="-fsanitize=memory"
LINKER_FLAGS="-Iinclude -lGL -lglfw -ldl -lm"

rm -f main
$CC $FILES $FLAGS $LINKER_FLAGS  && ./a.out
