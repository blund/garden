#!tcc -run

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

void  println(char *string);
char *run(char *command);
int   exec(char *command);
char *join(char *goal, char *part);

int main(int argc, char **argv) {
  
  char *compile      = "clang -o test main.c glad.c linalg.c helpers.c";
  char *flags        = "-D_REENTRANT";
  char *linker_flags = "-Iinclude/ -lGL -lSDL2 -ldl -lm";
  char *options      = "-march=native -mavx";

  int release = 0;

  if (argc > 1) {
    switch (*argv[1]) {
    case 's':
      println("Doing sanitizing");
      compile = join(compile, "-O2 -fsanitize=address");
      break;

    case 'r':
      println("Making release build");
      compile = join(compile, "-O2");
      release = 1;
      break;

    case 'o':
      println("Optimizing!");
      compile = join(compile, "-O3");
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
  compile = join(compile, options);
  compile = join(compile, run("sdl2-config --libs --cflags"));

  printf("\nCompiling with following command:\n\t%s\n", compile);

  
  int status = system(compile);
  if (status) {
    println("Failed to compile");
    return -1;
  }
  println("\tCompile success!");

  status = system("./test");
  if (status) {
    println("\tError during runtime!");
    return -1;
  }
  
  return 0;
}



void println(char *string) {
  printf("%s\n", string);
}

char *join(char *goal, char *part) {
  int length = strlen(goal) + strlen(part);
  char *new_str = (char *)calloc(length + 2, 1);
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

  char *result = (char *)calloc(strlen(tmp) + 1, 1);
  strcpy(result, tmp);

  return result;
}

int exec(char *command) {
  FILE *fp;
  
  fp = popen(command,"r"); 
  int code = pclose(fp);
  return code;
}
