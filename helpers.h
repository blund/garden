#ifndef HELPERS_H
#define HELPERS_H

#include "linalg.h"

char *loadFile(char *file_name);

int loadShader(char *shader_name, GLenum shader_type);

void setMat4(int shader, char *param, mat4 value);
void setVec3(int shader, char *param, vec3 value);
void setInt(int shader, char *param, int value);

#endif