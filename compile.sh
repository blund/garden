#!/bin/sh

CC="tcc"
FLAGS="-o build/garden -g"

MESSAGES=""

while getopts "c:o" arg; do
    case $arg in
        c)
            CC=${OPTARG}
            [[ ! $CC =~ clang|gcc ]] && CC=tcc
            ;;
        o)
            FLAGS="${FLAGS} -O2"
            ;;
        *)
            echo "Fikk et ulovlig argument: ${OPTARG}!"
            ;;
    esac
done
shift $((OPTIND-1))

echo
echo "Kompilerer med ${CC}"
echo

FILES="platform_code.c shader.c linalg.c helpers.c include/glad/glad.c"
SANITIZE="-fsanitize=address -fsanitize-blacklist=support/blacklist.txt"
LINKER_FLAGS="-Iinclude -lGL -lglfw -ldl -lm"


$CC $FILES $FLAGS $LINKER_FLAGS && ./build/garden
