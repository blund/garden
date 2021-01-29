#!/bin/sh

CC="clang"
FILES="game.c glad.c linalg.c helpers.c"
SANITIZE="-fsanitize=memory"
LINKER_FLAGS="-Iinclude -lGL -lglfw -ldl -lm"

$CC $FILES $FLAGS $LINKER_FLAGS  && ./a.out

#  int release = 0;
#
#  if (argc > 1) {
#    switch (*argv[1]) {
#    case 's':
#      println("Doing sanitizing");
#      compile = join(compile, "-O2 -fsanitize=address");
#      break;
#
#    case 'r':
#      println("Making release build");
#      compile = join(compile, "-O2");
#      release = 1;
#      break;
#
#    case 'o':
#      println("Optimizing!");
#      compile = join(compile, "-O3");
#      break;
#
#    default:
#      printf("Got bad parameter \"%c\"\n", *argv[1]);
#      return -1;
#    }
#  }
#  if (!release) {
#    compile = join(compile, "-ggdb");
#  }
#  
#  compile = join(compile, flags);
#  compile = join(compile, linker_flags);
#  compile = join(compile, options);
#  compile = join(compile, run("sdl2-config --libs --cflags"));
#
#  printf("\nCompiling with following command:\n\t%s\n", compile);
#
#  
#  int status = system(compile);
#  if (status) {
#    println("Failed to compile");
#    return -1;
#  }
#  println("\tCompile success!");
#
#  status = system("./test");
#  if (status) {
#    println("\tError during runtime!");
#    return -1;
#  }
#  
#  return 0;
