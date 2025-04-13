#ifndef LINALG_H
#define LINALG_H

typedef float mat4[4][4];
typedef float vec4[4];
typedef float vec3[3];

void vec4_to_vec3(const vec4 v4, vec3 v3);
void vec4_print(const vec4 v);
void vec3_normalize(const vec3 v, vec3 out);
void vec3_print(const vec3 v);
void vec3_copy(const vec3 v, vec3 out);
void vec3_add(const vec3 a, const vec3 b, vec3 out);
void vec3_sub(const vec3 a, const vec3 b, vec3 out);
void vec3_normalize(const vec3 v, vec3 out);
void vec3_cross(const vec3 a, const vec3 b, vec3 out);
float vec3_dot(const vec3 a, const vec3 b);
void mat4_identity(mat4 m);
void mat4_mul(mat4 result, mat4 a, mat4 b);
void mat4_rotate_x(mat4 m, float angle_rad);
void mat4_rotate_z(mat4 m, float angle_rad);
void mat4_lookat(vec3 eye, vec3 center, vec3 up, mat4 out);
void mat4_ortho(float left, float right, float bottom, float top, float near, float far, mat4 out);
void mat4_perspective(float fov_deg, float aspect, float near, float far, mat4 out);
void mat4_print(mat4 m);
void mat4_mul_vec4(const mat4 m, const vec4 v, vec4 out);
void mat4_scale(mat4 m, float sx, float sy, float sz);
void mat4_inverse(mat4 out, const mat4 m);
void mat4_translate(mat4 out, vec3 v);


#ifdef LINALG_IMPL

#include "stdio.h"
#include "math.h"
#include "string.h"

// vec3 funs
void vec4_to_vec3(const vec4 v4, vec3 v3) {
    v3[0] = v4[0];
    v3[1] = v4[1];
    v3[2] = v4[2];
}

void vec4_print(const vec4 v) {
  printf("vec4: [%.3f, %.3f, %.3f, %.3f]\n", v[0], v[1], v[2], v[3]);
}

void vec4_normalize(const vec4 v, vec4 out) {
    float length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
    if (length > 0.00001f) {
        out[0] = v[0] / length;
        out[1] = v[1] / length;
        out[2] = v[2] / length;
        out[3] = v[3] / length;
    } else {
        // fallback to zero vector if length is tiny
        out[0] = out[1] = out[2] = out[3] = 0.0f;
    }
}

void vec3_print(const vec3 v) {
    printf("vec3: [%.3f, %.3f, %.3f]\n", v[0], v[1], v[2]);
}

void vec3_add(const vec3 a, const vec3 b, vec3 out) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
}

void vec3_sub(const vec3 a, const vec3 b, vec3 out) {
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}

void vec3_normalize(const vec3 v, vec3 out) {
    float length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (length > 0.00001f) {
        out[0] = v[0] / length;
        out[1] = v[1] / length;
        out[2] = v[2] / length;
    } else {
        // fallback to zero vector if length is tiny
        out[0] = out[1] = out[2] = 0.0f;
    }
}

void vec3_cross(const vec3 a, const vec3 b, vec3 out) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

float vec3_dot(const vec3 a, const vec3 b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


void vec3_copy(const vec3 v, vec3 out) {
  memcpy(out, v, sizeof(vec3));
}

void vec3_scale(const vec3 v, float s, vec3 out) {
    out[0] = v[0] * s;
    out[1] = v[1] * s;
    out[2] = v[2] * s;
}

// mat4 funs

void mat4_identity(mat4 m) {
    memset(m, 0, sizeof(mat4));
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

void mat4_mul(mat4 result, mat4 a, mat4 b) {
    mat4 temp;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col) {
            temp[row][col] = 0.0f;
            for (int k = 0; k < 4; ++k)
                temp[row][col] += a[row][k] * b[k][col];
        }
    memcpy(result, temp, sizeof(mat4));
}

void mat4_rotate_x(mat4 m, float angle_rad) {
    mat4 rot;
    mat4_identity(rot);
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    rot[1][1] = c;
    rot[1][2] = -s;
    rot[2][1] = s;
    rot[2][2] = c;
    mat4_mul(m, rot, m);
}

void mat4_rotate_z(mat4 m, float angle_rad) {
    mat4 rot;
    mat4_identity(rot);
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    rot[0][0] = c;
    rot[0][1] = -s;
    rot[1][0] = s;
    rot[1][1] = c;
    mat4_mul(m, rot, m);
}

void mat4_lookat(vec3 eye, vec3 center, vec3 up, mat4 out) {
    vec3 f, s, u;
    vec3_sub(center, eye, f);
    vec3_normalize(f, f);

    vec3_cross(f, up, s);
    vec3_normalize(s, s);

    vec3_cross(s, f, u);

    mat4_identity(out);
    out[0][0] = s[0];
    out[1][0] = s[1];
    out[2][0] = s[2];
    
    out[0][1] = u[0];
    out[1][1] = u[1];
    out[2][1] = u[2];

    out[0][2] = -f[0];
    out[1][2] = -f[1];
    out[2][2] = -f[2];

    out[3][0] = -vec3_dot(s, eye);
    out[3][1] = -vec3_dot(u, eye);
    out[3][2] =  vec3_dot(f, eye); // + because we negated f earlier
}

void mat4_inverse_lookat(vec3 eye, vec3 center, vec3 up, mat4 out) {
    vec3 f, s, u;
    vec3_sub(center, eye, f);
    vec3_normalize(f, f);

    vec3_cross(f, up, s);
    vec3_normalize(s, s);

    vec3_cross(s, f, u);

    mat4_identity(out);

    // Rotation part (transpose of view matrix rotation)
    out[0][0] = s[0];
    out[0][1] = s[1];
    out[0][2] = s[2];

    out[1][0] = u[0];
    out[1][1] = u[1];
    out[1][2] = u[2];

    out[2][0] = -f[0];
    out[2][1] = -f[1];
    out[2][2] = -f[2];

    // Translation (put camera back in world space)
    out[3][0] = eye[0];
    out[3][1] = eye[1];
    out[3][2] = eye[2];
}

void mat4_ortho(float left, float right, float bottom, float top, float near, float far, mat4 out) {
    mat4_identity(out);

    out[0][0] = 2.0f / (right - left);
    out[1][1] = 2.0f / (top - bottom);
    out[2][2] = -2.0f / (far - near);

    out[3][0] = -(right + left) / (right - left);
    out[3][1] = -(top + bottom) / (top - bottom);
    out[3][2] = -(far + near) / (far - near);
}

void mat4_perspective(float fov_deg, float aspect, float near, float far, mat4 out) {
    float fov_rad = fov_deg * (M_PI / 180.0f);
    float f = 1.0f / tanf(fov_rad / 2.0f);

    mat4_identity(out);
    out[0][0] = f / aspect;
    out[1][1] = f;
    out[2][2] = (far + near) / (near - far);
    out[2][3] = -1.0f;
    out[3][2] = (2.0f * far * near) / (near - far);
    out[3][3] = 0.0f;
}

void mat4_print(mat4 m) {
    for (int i = 0; i < 4; i++) {
        printf("| %6.2f %6.2f %6.2f %6.2f |\n", m[0][i], m[1][i], m[2][i], m[3][i]);
    }
}

void mat4_mul_vec4(const mat4 m, const vec4 v, vec4 out) {
    for (int i = 0; i < 4; ++i) {
        out[i] = m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2] + m[i][3] * v[3];
    }
}

void mat4_scale(mat4 m, float sx, float sy, float sz) {
    m[0][0] *= sx;
    m[1][1] *= sy;
    m[2][2] *= sz;
}

void mat4_inverse(mat4 out, const mat4 m) {
    float inv[16], det;
    float *a = (float*)m;

    inv[0]  =  a[5]*a[10]*a[15] - a[5]*a[11]*a[14] - a[9]*a[6]*a[15]
             + a[9]*a[7]*a[14] + a[13]*a[6]*a[11] - a[13]*a[7]*a[10];

    inv[1]  = -a[1]*a[10]*a[15] + a[1]*a[11]*a[14] + a[9]*a[2]*a[15]
             - a[9]*a[3]*a[14] - a[13]*a[2]*a[11] + a[13]*a[3]*a[10];

    inv[2]  =  a[1]*a[6]*a[15] - a[1]*a[7]*a[14] - a[5]*a[2]*a[15]
             + a[5]*a[3]*a[14] + a[13]*a[2]*a[7] - a[13]*a[3]*a[6];

    inv[3]  = -a[1]*a[6]*a[11] + a[1]*a[7]*a[10] + a[5]*a[2]*a[11]
             - a[5]*a[3]*a[10] - a[9]*a[2]*a[7] + a[9]*a[3]*a[6];

    inv[4]  = -a[4]*a[10]*a[15] + a[4]*a[11]*a[14] + a[8]*a[6]*a[15]
             - a[8]*a[7]*a[14] - a[12]*a[6]*a[11] + a[12]*a[7]*a[10];

    inv[5]  =  a[0]*a[10]*a[15] - a[0]*a[11]*a[14] - a[8]*a[2]*a[15]
             + a[8]*a[3]*a[14] + a[12]*a[2]*a[11] - a[12]*a[3]*a[10];

    inv[6]  = -a[0]*a[6]*a[15] + a[0]*a[7]*a[14] + a[4]*a[2]*a[15]
             - a[4]*a[3]*a[14] - a[12]*a[2]*a[7] + a[12]*a[3]*a[6];

    inv[7]  =  a[0]*a[6]*a[11] - a[0]*a[7]*a[10] - a[4]*a[2]*a[11]
             + a[4]*a[3]*a[10] + a[8]*a[2]*a[7] - a[8]*a[3]*a[6];

    inv[8]  =  a[4]*a[9]*a[15] - a[4]*a[11]*a[13] - a[8]*a[5]*a[15]
             + a[8]*a[7]*a[13] + a[12]*a[5]*a[11] - a[12]*a[7]*a[9];

    inv[9]  = -a[0]*a[9]*a[15] + a[0]*a[11]*a[13] + a[8]*a[1]*a[15]
             - a[8]*a[3]*a[13] - a[12]*a[1]*a[11] + a[12]*a[3]*a[9];

    inv[10] =  a[0]*a[5]*a[15] - a[0]*a[7]*a[13] - a[4]*a[1]*a[15]
             + a[4]*a[3]*a[13] + a[12]*a[1]*a[7] - a[12]*a[3]*a[5];

    inv[11] = -a[0]*a[5]*a[11] + a[0]*a[7]*a[9] + a[4]*a[1]*a[11]
             - a[4]*a[3]*a[9] - a[8]*a[1]*a[7] + a[8]*a[3]*a[5];

    inv[12] = -a[4]*a[9]*a[14] + a[4]*a[10]*a[13] + a[8]*a[5]*a[14]
             - a[8]*a[6]*a[13] - a[12]*a[5]*a[10] + a[12]*a[6]*a[9];

    inv[13] =  a[0]*a[9]*a[14] - a[0]*a[10]*a[13] - a[8]*a[1]*a[14]
             + a[8]*a[2]*a[13] + a[12]*a[1]*a[10] - a[12]*a[2]*a[9];

    inv[14] = -a[0]*a[5]*a[14] + a[0]*a[6]*a[13] + a[4]*a[1]*a[14]
             - a[4]*a[2]*a[13] - a[12]*a[1]*a[6] + a[12]*a[2]*a[5];

    inv[15] =  a[0]*a[5]*a[10] - a[0]*a[6]*a[9] - a[4]*a[1]*a[10]
             + a[4]*a[2]*a[9] + a[8]*a[1]*a[6] - a[8]*a[2]*a[5];

    det = a[0]*inv[0] + a[1]*inv[4] + a[2]*inv[8] + a[3]*inv[12];
    if (det == 0.0f) {
      printf("oopsie from mat4_inverse\n");
      return;
    }

    det = 1.0f / det;

    // Copy result back into out (row-major layout)
    for (int i = 0; i < 16; i++)
        ((float*)out)[i] = inv[i] * det;
}

void mat4_translate(mat4 out, vec3 v) {
    // Identity matrix
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[i][j] = (i == j) ? 1.0f : 0.0f;

    // Set translation column
    out[3][0] = v[0];
    out[3][1] = v[1];
    out[3][2] = v[2];
}


#endif
#endif
