#include <malloc.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include <string.h>
#include <stdlib.h>

#include <stb/stb_image.h> // Laster kun inn funksjonsdeklarasjoner

#include "helpers.h"
#include "linalg.h"


char *loadFile(const char *file_name) {
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
 
  /* reset the file position indicator to 
     the beginning of the file */
  fseek(infile, 0L, SEEK_SET);	
 
  /* grab sufficient memory for the 
     buffer to hold the text */
  //buffer = (char*)calloc(numbytes+1,1);
  buffer = (char *)malloc(numbytes+1);
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
}


int loadTexture(char *texture_path) {
  //
  // Load texture file
  //
  glActiveTexture(GL_TEXTURE0);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nr_channels;
  unsigned char *data = stbi_load(texture_path, &width, &height, &nr_channels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Klarte ikke å laste inn bildet ved %s!\n", texture_path);
    return 0;
  }

  stbi_image_free(data);

  return texture;
}

float min(float x, float y) {
  if (x < y) return x;
  return y;
}
