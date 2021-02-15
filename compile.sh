#!/bin/sh


CC="tcc"
while getopts "c:" arg; do
    case $arg in
        c)
            CC=${OPTARG}
            [[ ! $CC =~ clang|gcc ]] && CC=tcc
            ;;
        *)
            echo "bad argument!"
            ;;
    esac
done
shift $((OPTIND-1))


FILES="game.c glad.c linalg.c helpers.c mu/renderer.c mu/microui.c"
FLAGS="-g"
SANITIZE="-fsanitize=address -fsanitize-blacklist=support/blacklist.txt"
LINKER_FLAGS="-Iinclude -lGL -lglfw -ldl -lm"

echo "Kompilerer med ${CC}"
$CC $FILES $FLAGS $LINKER_FLAGS  && ./a.out
