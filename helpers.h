#ifndef HELPERS_H
#define HELPERS_H

#include <GL/gl.h>
#include "linalg.h"

#define s8  int8_t
#define u8  uint8_t
#define s16 int16_t
#define u16 uint16_t
#define s32 int32_t
#define u32 uint32_t

#define true 1
#define false 0


char *loadFile(const char *file_name);

int loadTexture(char *texture_path);
  
//int createShader(char *fs_path, char *vs_path);
//int loadShader(char *shader_name, int shader_type);

void setMat4(int shader, char *param, mat4 value);
void setVec3(int shader, char *param, vec3 value);
void setInt(int shader, char *param, int value);

#endif
