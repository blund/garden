#!tcc -run
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

void println(char *string);
char *run(char *command);
char *join(char *goal, char *part);

int main(int argc, char **argv) {
  
  char *compile = "clang main.c glad.c linalg.c helpers.c";
  char *flags = "-o test -D_REENTRANT";
  char *linker_flags = "-Iinclude -lGL -lSDL2 -ldl -lm";
  char *options = "-march=native -mavx ";

  int release = 0;

  if (argc > 1) {
    switch (*argv[1]) {
    case 's':
      println("Doing sanitizing");
      compile = join(compile, "-fsanitize=address");
      break;

    case 'r':
      println("Making release build");
      compile = join(compile, "-O2");
      release = 1;
      break;

    case 'o':
      println("Optimizing!");
      compile = join(compile, "-O2");
      break;

    default:
      printf("Got bad parameter \"%c\"\n", *argv[1]);
      return -1;
    }
  }

  if (!release) {
  compile = join(compile, "-ggdb");
  }

  compile = join(compile, flags);
  compile = join(compile, linker_flags);
  compile = join(compile, run("pkg-config --cflags sdl2")); // sdl flags
  compile = join(compile, run("pkg-config --libs   sdl2")); // sdl libs

  printf("\nCompiling with following command:\n\t%s\n", compile);

  int status = system(compile);
  if (status) {
    println("Failed to compile");
    return -1;
  }
  println("\tCompile success!");
  run("./test");
  
  return 0;
}

void println(char *string) {
  printf("%s\n", string);
}

char *join(char *goal, char *part) {
  int length = strlen(goal) + strlen(part);
  char *new_str = (char *)malloc(length + 1);
  strcpy(new_str, goal);
  new_str[strlen(goal)] = 32;
  strcpy(new_str+strlen(goal)+1, part);
  return new_str;
}

char *run(char *command) {
  FILE *fp;
  
  fp = popen(command,"r"); 
  char tmp[1024];
  fscanf(fp,"%s", tmp);
  fclose(fp);

  char *result = (char *)malloc(strlen(tmp) + 1);
  strcpy(result, tmp);

  return result;

}
