// https://www.3dgep.com/understanding-quaternions/
#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#ifdef __SSE__
#include <immintrin.h>
#endif

#include "linalg.h"





/*

  @TODO - bytt ut rotateX/Y/Z med X2/Y2-versjonene, som er lettere å bruke siden dette så å si er gratis kanskje :):)



 */



// http://scipp.ucsc.edu/~haber/ph216/rotation_12.pdf
// Rotasjon om vilkårlig akse!!

/*
  Merk her at matriser er "column major", altså
  at man traverserer fra første kolonne og ned, så neste kolonne og ned..
  Vi bruker en interface som gjør dette litt lettere :)
  (mset og mgdet bruker rad-kolonne-indeksering)

 */

//
// Quaternion
//



void addQt (Qt *a, Qt *b) {
  for (int i = 0; i < 4; i++) {
    a->array[i] += b->array[i];
  }
}

void subQt (Qt *a, Qt *b) {
  for (int i = 0; i < 4; i++) {
    a->array[i] -= b->array[i];
  }
}

void mulQt (Qt *a_, Qt *b) {
  // https://www.dgep.com/understanding-quaternions/#Quaternions
  Qt a = {a_->s, a_->x, a_->y, a_->z};
  a_->s = (a.s*b->s) - (a.x*b->x) - (a.y*b->y) - (a.z*b->z);
  a_->x = (a.s*b->x) + (b->s*a.x) + (a.y*b->z) - (b->y*a.z);
  a_->y = (a.s*b->y) + (b->s*a.y) + (a.z*b->x) - (b->z*a.x);
  a_->z = (a.s*b->z) + (b->s*a.z) + (a.x*b->y) - (b->x*a.y);
}


void mulV3Qt (vec3 v, Qt *q) {
  // https://math.stackexchange.com/questions/40164/how-do-you-rotate-a-vector-by-a-unit-quaternion
  // vec3 test = {1.0f, 0.0f, 0.0f};
  // Qt   rot  = {0.707f, 0.0f, 0.707f, 0.0f};

  // mulV3Qt(test, &rot);
  // normalizeV3(test);
  // printV3(test);
  
  Qt rot  = {q->s, q->x, q->y, q->z};
  Qt vec  = {0.0f, v[0], v[1], v[2]};
  Qt rot_ = {q->s, -q->x, -q->y, -q->z};

  
  mulQt(&rot, &vec);
  mulQt(&rot, &rot_);

  v[0] = rot.x; v[1] = rot.y; v[2] = rot.z;
}


void scaleQt (Qt *a, float scale) {
  for (int i = 0; i < 4; i++) {
    a->array[i] *= scale;
  }
}


void normalizeQt (Qt *q) {
  //

  float norm = sqrt(pow(q->array[0], 2) + pow(q->array[1],2) + pow(q->array[2],2) + pow(q->array[3],2));

  for (int i = 0; i < 4; i++) {
    q->array[i] /= norm;
  }

}

void printQt(Qt a) {
  printf("Qt: %f %f %f %f\n", a.s, a.x, a.y, a.z);
}



//
// Matriser
//


void inline mset(mat4 m, int row, int col, float val) {
  m[row + 4*col] = val;
}


float inline mget(mat4 m, int row, int col) {
  return m[row + 4*col];
}

  
void initIdM4(mat4 m) {
  clearM4(m);
  mset(m, 0, 0, 1.0f);
  mset(m, 1, 1, 1.0f);
  mset(m, 2, 2, 1.0f);
  mset(m, 3, 3, 1.0f);
}

void inline setV3(vec3 v, int a, int b, int c) {
  v[0] = a;
  v[1] = b;
  v[2] = c;
}

void mkTranslation(mat4 m, vec3 v) {
  mset(m, 0, 3, v[0]);
  mset(m, 1, 3, v[1]);
  mset(m, 2, 3, v[2]);
}

void translate(mat4 m, vec3 v, mat4 res, mat4 tmp) {
  initIdM4(tmp);
  mkTranslation(tmp, v);
  mulM4(m, tmp, res);
}

void clearM4(mat4 m) {
  for (int i = 0; i < 16; i++) {
    m[i] = 0.0f;
  }
}

void copyM4(mat4 from, mat4 to) {
  for (int i = 0; i < 16; i++) {
    to[i] = from[i];
  }
}

/*
void mulM4(mat4 A, mat4 B, mat4 result) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; i < 4; i++) {
      mset(result, i, j,
           mget(A, i, 1) * mget(B, 1, j)
           mget(A, i, 2) * mget(B, 2, j)
           mget(A, i, 3) * mget(B, 3, j)
           mget(A, i, 4) * mget(B, 4, j));
    }
  }
}
*/


void ortho(float left, float right, float bottom, float top,
           float near, float far, mat4 res) {
  mset(res, 0, 0,
       2.0f / (right - left));
  mset(res, 1, 1,
       2.0f / (top - bottom));
  mset(res, 2, 2,
       - 2.0f / (far - near));
  mset(res, 3, 0,
       -(right + left) / (right - left));
  mset(res, 3, 1,
       - (top + bottom) / (top - bottom));
  mset(res, 3, 2,
       -(far + near) / (far - near));
}


// projeksjonsmatrise
// http://www.songho.ca/opengl/gl_projectionmatrix.html
void perspective(float fov, float aspect, float near, float far, mat4 res) {
  //assert(aspect != 0.0f);
  //assert(far != near);

  float tanHalfFovy = tan(fov / 2.0f);

  mset(res, 0, 0,
       1.0f / (aspect * tanHalfFovy));
  mset(res, 1, 1,
       1.0f / (tanHalfFovy));
  mset(res, 2, 2,
       - (far + near) / (far - near));
  mset(res, 3, 2,
       - 1.0f);
  mset(res, 2, 3,
       - (2.0f * far * near) / (far - near));
}


void mulM4_(mat4 m1, mat4 m2) {
  mat4 tmp;
  copyM4(m1, tmp);
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m1[j + i * 4] = 0;
      for (int k = 0; k < 4; k++) {
        m1[j + i * 4] += tmp[k + i * 4] * m2[j + k * 4];
      }
    }
  }
}

void mulM4(mat4 m1, mat4 m2, mat4 res) {
  

  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      res[j + i * 4] = 0;
      for (int k = 0; k < 4; k++) {
        res[j + i * 4] += m1[k + i * 4] * m2[j + k * 4];
      }
    }
  }
}

void rotateZ(mat4 m, float deg, mat4 res) {
  float rot[16] = {
    cos(deg), sin(deg), 0, 0,
    -sin(deg), cos(deg),  0, 0,
    0.0f, 0.0f,    1.0f,     0.0f,
    0.0f, 0.0f,    0.0f,     1.0f,
  };

  mulM4(m, rot, res);
}

void rotateX(mat4 m, float deg, mat4 res) {
  float rot[16] = {
    1.0f, 0,        0,         0,
    0,    cos(deg), sin(deg), 0,
    0,    -sin(deg), cos(deg),  0, 
    0.0f, 0.0f,     0.0f,      1.0f,
  };

  mulM4(rot, m, res);
}

void rotateX2(mat4 m, float deg) {
  float rot[16] = {
    1.0f,   0.0f,     0.0f,      0.0f,
    0.0f,   cos(deg), sin(deg),  0.0f,
    0.0f,  -sin(deg), cos(deg),  0.0f, 
    0.0f,   0.0f,     0.0f,      1.0f,
  };

  mulM4_(m, rot);
}

void rotateY(mat4 m, float deg, mat4 res) {
  float rot[16] = {
    cos(deg),  0.0f, sin(deg),  0.0f,
    0.0f,      1.0f, 0.0f,      0.0f,
    -sin(deg), 0.0f, cos(deg),  0.0f, 
    0.0f,      0.0f, 0.0f,      1.0f,
  };

  mulM4(rot, m, res);
}

void rotateY2(mat4 m, float deg) {
  float rot[16] = {
    cos(deg),  0.0f, sin(deg),  0.0f,
    0.0f,      1.0f, 0.0f,      0.0f,
    -sin(deg), 0.0f, cos(deg),  0.0f, 
    0.0f,      0.0f, 0.0f,      1.0f,
  };
  
  mulM4_(m, rot);
}

void scaleM4(mat4 m, vec3 v) {
  mat4 tmp;
  copyM4(m, tmp);
  
  mat4 scale = {};
  mset(scale, 0, 0, v[0]);
  mset(scale, 1, 1, v[1]);
  mset(scale, 2, 2, v[2]);
  mset(scale, 3, 3, 1.0f);
  mulM4(tmp, scale, m);
}


/*
inline float rsqrt( float number ) {
  //#ifdef __AVX__
  //      return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ps1(number)));
  //#else
  long i;
  float x2, y;
  const float threehalfs = 1.5F;
  
  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
  
  return y;
  //#endif
}
*/


float lenV3(vec3 v) {
  return sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
}

void normalizeV3(vec3 v) {
  float factor = 1/sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2)); //rsqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
  v[0] *= factor;
  v[1] *= factor;
  v[2] *= factor;
}

void crossV3(vec3 u, vec3 v, vec3 result) {
  result[0] = u[1]*v[2] - u[2]*v[1];
  result[1] = u[2]*v[0] - u[0]*v[2];
  result[2] = u[0]*v[1] - u[1]*v[0];
}

float dotV3(vec3 u, vec3 v) {
  return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

float angleV3(vec3 u, vec3 v) {
  float len_u = sqrt(pow(u[0],2) + pow(u[1],2) + pow(u[2],2));
  float len_v = sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));

  float cos_theta = dotV3(u, v)/(len_u * len_v);
  return acos(cos_theta);  
}

void scaleV3(vec3 v, float f) {
  v[0] *= f;
  v[1] *= f;
  v[2] *= f;
}

void copyV3(vec3 from, vec3 to) {
  for (int i = 0; i < 3; i++) {
    to[i] = from[i];
  }
}

void addV3(vec3 to, vec3 from) {
  for (int i = 0; i < 3; i++) {
    to[i] += from[i];
  }
}

void subV3(vec3 to, vec3 from) {
  for (int i = 0; i < 3; i++) {
    to[i] -= from[i];
  }
}


void printV3(vec3 v) {
  printf("[%f, %f, %f]\n", v[0], v[1], v[2]);
}
