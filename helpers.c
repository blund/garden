#include <malloc.h>
#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <stdlib.h>

#include "helpers.h"
#include "linalg.h"


char *loadFile(char *file_name)
{
  /* declare a file pointer */
  FILE    *infile;
  char    *buffer;
  long    numbytes;
 
  /* open an existing file for reading */
  infile = fopen(file_name, "r");
 
  /* quit if the file does not exist */
  // if(infile == NULL)
  //   return 1;
 
  /* Get the number of bytes */
  fseek(infile, 0L, SEEK_END);
  numbytes = ftell(infile);
  printf("%li\n", numbytes);
 
  /* reset the file position indicator to 
     the beginning of the file */
  fseek(infile, 0L, SEEK_SET);	
 
  /* grab sufficient memory for the 
     buffer to hold the text */
  //buffer = (char*)calloc(numbytes+1,1);
  buffer = (char *)malloc(1024);
  buffer[numbytes] = 0;
 
  /* memory error */
  // if(buffer == NULL)
  //   return 1;
 
  /* copy all the text into the buffer */
  fread(buffer, sizeof(char), numbytes, infile);
  fclose(infile);
 
  /* confirm we have read the file by
     outputing it to the console */
 
  /* free the memory we used for the buffer */
  return buffer;
  //free(buffer);


}

int loadShader(char *shader_name, GLenum shader_type) {
  // Last inn og kompiler fragment-shader-program
  int shader_id = glCreateShader(shader_type);
  const char *shader_source = loadFile(shader_name);
  int len = strlen(shader_source);
  printf("%lu\n", strlen(shader_source));
  glShaderSource(shader_id, 1, &shader_source, &len);
  glCompileShader(shader_id);

  // check for shader compile errors
  char infoLog[512];
  int success;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
    printf("Failed to compile shader: %s\n", shader_name);
    printf("\t%s", infoLog);
    return -1;
  }
  free((char *)shader_source);

  return shader_id;

}

void inline setMat4(int shader, char *param, mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader, param), 1, GL_FALSE, value);
}

void inline setInt(int shader, char *param, int value) {
  glUniform1i(glGetUniformLocation(shader, param), value);
}


void inline setVec3(int shader, char *param, vec3 value) {
  glUniform3fv(glGetUniformLocation(shader, param), 1, value);
}