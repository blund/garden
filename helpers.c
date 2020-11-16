#include <malloc.h>
#include <sys/stat.h>



char *loadFile(char *file_name)
{
  struct stat st;

  stat(file_name, &st);
  int file_size = st.st_size;

  char *file_pointer = (char *) malloc(file_size);
  
  FILE *file = fopen( file_name, "r" );

  fread( file_pointer, 1, file_size, file );

  fclose( file );
  return file_pointer;
}

