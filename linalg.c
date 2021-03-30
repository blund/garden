// https://www.3dgep.com/understanding-quaternions/
#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#ifdef __SSE__
#include <immintrin.h>
#endif

#include "linalg.h"


// @TODO - endre mat4-greien til å bruke V3, også Qt-greiene.


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


Qt realQt() {
  Qt out = {1.0f};
  return out;
}


Qt addQt (Qt a, Qt b) {
  Qt out;
  for (int i = 0; i < 4; i++) {
    out.arr[i] = a.arr[i] + b.arr[i];
  }
  return out;
}


Qt subQt (Qt a, Qt b) {
  Qt out;
  for (int i = 0; i < 4; i++) {
    out.arr[i] = a.arr[i] - b.arr[i];
  }
  return out;
}

Qt mulQt (Qt a, Qt b) {
  // https://www.dgep.com/understanding-quaternions/#Quaternions
  Qt out;
  out.s = (a.s*b.s) - (a.x*b.x) - (a.y*b.y) - (a.z*b.z);
  out.x = (a.s*b.x) + (b.s*a.x) + (a.y*b.z) - (b.y*a.z);
  out.y = (a.s*b.y) + (b.s*a.y) + (a.z*b.x) - (b.z*a.x);
  out.z = (a.s*b.z) + (b.s*a.z) + (a.x*b.y) - (b.x*a.y);

  return out;
}

void QtAsM4 (Qt q, mat4 m) {
  // q MÅ NORMALISERES FØRST
  initIdM4(m);
  mset(m, 0,0, 1.0f - 2*pow(q.y,2) -  2*pow(q.z,2));
  mset(m, 0,1, 2*q.x*q.y - 2*q.z*q.s);
  mset(m, 0,2, 2*q.x*q.z + 2*q.y*q.s);
  mset(m, 1,0, 2*q.x*q.y + 2*q.z*q.s);
  mset(m, 1,1, 1.0f - 2*pow(q.x,2) -  2*pow(q.z,2));
  mset(m, 1,2, 2*q.y*q.z - 2*q.x*q.s);
  mset(m, 2,0, 2*q.x*q.z - 2*q.y*q.s);
  mset(m, 2,1, 2*q.y*q.z + 2*q.x*q.s);
  mset(m, 2,2, 1.0f - 2*pow(q.x,2) - 2*pow(q.y,2));
}


Qt rotateV3Qt (V3 v, Qt q) {
  // Hamilton multiplication https://math.stackexchange.com/questions/40164/how-do-you-rotate-a-vector-by-a-unit-quaternion

  Qt rot     = {q.s, q.x, q.y, q.z};
  Qt vec     = {0.0f, v.x, v.y, v.z};
  Qt rot_inv = {q.s, -q.x, -q.y, -q.z};

  return mulQt(mulQt(rot, vec), rot_inv);
}

Qt scaleQt (Qt in, float scale) {
  Qt out;
  for (int i = 0; i < 4; i++) {
    out.arr[i] = scale * in.arr[i];
  }
  return out;
}

Qt normalizeQt (Qt q) {
  Qt out;
  // @TODO - erstatt med compiler intrinsics, for clang og gcc
  float norm = sqrt(pow(q.s, 2) + pow(q.x,2) + pow(q.y,2) + pow(q.z,2));

  for (int i = 0; i < 4; i++) {
    out.arr[i] = q.arr[i] / norm;
  }

  return out;

}

Qt rotationQt(float theta, V3 dir) {
  Qt qt;
  float sin_theta = sin(theta);
  qt.s = cos(theta);
  qt.x = dir.x*sin_theta;
  qt.y = dir.y*sin_theta;
  qt.z = dir.z*sin_theta;

  return normalizeQt(qt);
}

Qt rotationQtfs(float theta, float x, float y, float z) {
  Qt qt;

  theta /= 2.0f;
  float sin_theta = sin(theta);
  qt.s = cos(theta);
  qt.x = x*sin_theta;
  qt.y = y*sin_theta;
  qt.z = z*sin_theta;

  return normalizeQt(qt);
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

void inline setV3(V3 v, float a, float b, float c) {
  v.x = a;
  v.y = b;
  v.z = c;
}

void transM4(mat4 m, V3 v) {
  m[12] += v.x;
  m[13] += v.y;
  m[14] += v.z;
}

void translate(mat4 m, V3 v, mat4 res, mat4 tmp) {
  initIdM4(tmp);
  transM4(tmp, v);
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

void scaleM4(mat4 m, V3 v) {
  mat4 tmp;
  copyM4(m, tmp);
  
  mat4 scale = {};
  mset(scale, 0, 0, v.x);
  mset(scale, 1, 1, v.y);
  mset(scale, 2, 2, v.z);
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


V3 mkV3(float x, float y, float z) {
  V3 result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

V3 addV3(V3 a, V3 b) {
  V3 v_out;
  for (int i = 0; i < 3; i++) {
    v_out.arr[i] = a.arr[i] + b.arr[i];
  }
  return v_out;
}

V3 subV3(V3 a, V3 b) {
  V3 v_out;
  for (int i = 0; i < 3; i++) {
    v_out.arr[i] = a.arr[i] - b.arr[i];
  }
  return v_out;
}

V3 crossV3(V3 u, V3 v) {
  V3 out;
  out.x = u.y*v.z - u.z*v.y;
  out.y = u.z*v.x - u.x*v.z;
  out.z = u.x*v.y - u.y*v.x;

  return out;
}

V3 scaleV3(V3 v_in, float f) {
  V3 v_out;
  for (int i = 0; i < 3; i++) {
    v_out.arr[i] = v_in.arr[i] * f;
  }
  return v_out;
}

float dotV3(V3 u, V3 v) {
  return u.x*v.x + u.y*v.y + u.z*v.z;
}

float angleV3(V3 u, V3 v) {
  float len_u = sqrt(pow(u.x,2) + pow(u.y,2) + pow(u.z,2));
  float len_v = sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));

  float cos_theta = dotV3(u, v)/(len_u * len_v);
  return acos(cos_theta);  
}

V3 normalizeV3(V3 v) {
  float factor = 1/sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2)); //rsqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
  V3 out;
  for (int i = 0; i < 3; i++) {
    out.arr[i] = factor * v.arr[i];
  }
  return out;
}

float lenV3(V3 v) {
  return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
}

