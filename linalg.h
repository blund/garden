#ifndef LINALG_H
#define LINALG_H

typedef float mat4[16];
typedef float vec3[3];
// @TODO ordne finere definisjon hvor vi kan bruke x, y, z...
/*
typedef struct {
  union {
    struct { float x, y, z };
    float v[3];
  };
} vec3;
*/

// Matrices
void initIdM4(mat4 m);

void clearM4(mat4 m);
void copyM4(mat4 from, mat4 to);

void mset(mat4 m1, int row, int col, float val);
float mget(mat4 m1, int row, int col);

void mkTranslation(mat4 m, vec3 v);
void rotateZ(mat4 m, float deg, mat4 res);
void rotateX(mat4 m, float deg, mat4 res);
void rotateY(mat4 m, float deg, mat4 res);
void translate(mat4 m, vec3 v, mat4 res, mat4 tmp);
void multiply(mat4 m1, mat4 m2, mat4 res);


void ortho(float left, float right, float bottom, float top,
           float near, float far, mat4 res);
void perspective(float fov, float aspect, float near, float far, mat4 res);

// Vectors
void normalizeV3(vec3 v);
void setV3(vec3 v, int a, int b, int c);
void scaleV3(vec3 v, float f);
void addV3(vec3 to, vec3 from);
void subV3(vec3 to, vec3 from);
void copyV3(vec3 from, vec3 to);
void crossV3(vec3 u, vec3 v, vec3 result);
void normalizeV3(vec3 v);
void printV3(vec3 v);
float angleV3(vec3 u, vec3 v);

// Various
float rsqrt(float number);
#endif
