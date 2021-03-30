#ifndef LINALG_H
#define LINALG_H

typedef float mat4[16];

typedef struct V3 {
  union {
    float arr[3];
    struct {
      float x;
      float y;
      float z;
    };
  };
} V3;


// Quaternions
typedef struct Qt {
  union {
    float arr[4];
    struct {
      float s;
      float x;
      float y;
      float z;
    };
    struct {
      float w;
      float i;
      float j;
      float k;
    };
  };
} Qt;


Qt realQt();
Qt addQt (Qt a, Qt b);
Qt subQt (Qt a, Qt b);
Qt mulQt (Qt a, Qt b);
Qt normalizeQt  (Qt a);
Qt rotateV3Qt   (V3 v, Qt q);
Qt rotationQt   (float angle, V3 dir); // normaliserer dir
Qt rotationQtfs (float theta, float x, float y, float z);
void printQt (Qt a);
void QtAsM4  (Qt q, mat4 m);


// Matrices
void initIdM4(mat4 m);

void clearM4(mat4 m);
void copyM4(mat4 from, mat4 to);

void  mset(mat4 m1, int row, int col, float val);
float mget(mat4 m1, int row, int col);

void mulM4_(mat4 m1, mat4 m2);
void transM4(mat4 m, V3 v);
void rotateZ(mat4 m, float deg, mat4 res);
void rotateX(mat4 m, float deg, mat4 res);
void rotateY(mat4 m, float deg, mat4 res);
void rotateX2(mat4 res, float deg);
void rotateY2(mat4 res, float deg);
void translate(mat4 m, V3 v, mat4 res, mat4 tmp);
void mulM4(mat4 m1, mat4 m2, mat4 res);
void scaleM4(mat4 m, V3 v);

void ortho(float left, float right, float bottom, float top,
           float near, float far, mat4 res);
void perspective(float fov, float aspect, float near, float far, mat4 res);

// Vectors
V3    mkV3(float a, float b, float c);
V3    addV3(V3 to, V3 from);
V3    subV3(V3 to, V3 from);
V3    scaleV3(V3 v, float f);
V3    crossV3(V3 u, V3 v);
V3    normalizeV3(V3 v);
float angleV3(V3 u, V3 v);
float dotV3(V3 u, V3 v);
float lenV3(V3 v);

void  printV3(V3 v);

// Various
float rsqrt(float number);
#endif
