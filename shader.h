#ifndef SHADER_H
#define SHADER_H


typedef struct Shader {
  const char *name;  
  int   id;

  const char *v_path; // Må være gitt for å kunne kompilere
  const char *f_path; // Må være gitt for å kunne kompilere
  
  int v_modified;
  int f_modified;
} Shader;

int loadShader(const char *shader_name, int shader_type);
int compileShader(Shader *shader, int verbose);
int hotloadShader(Shader *shader);

void setMat4 (int shader, char *param, mat4 value);
void setVec3 (int shader, char *param, V3 value);
void setInt  (int shader, char *param, int value);
void setFloat(int shader, char *param, float value);


#endif
