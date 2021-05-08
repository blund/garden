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

float randFloat(float min, float max);
V3    randV3(float min, float max);

char *loadFile(const char *file_name);
int   loadTexture(char *texture_path);
  

float min(float x, float y);

#endif
