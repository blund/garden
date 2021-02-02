

// https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideoopengl.html
// https://wiki.libsdl.org/SDL_GL_CreateContext
// https://www.glfw.org/docs/latest/compile.html#compile_manual
// https://lazyfoo.net/tutorials/SDL/index.php

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <time.h>

#ifndef __AVX__
  #define STBI_NO_SIMD
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "linalg.h"
#include "helpers.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

typedef struct controller {
  int up    : 1;
  int down  : 1;
  int left  : 1;
  int right : 1;

  int one   : 1;
  int two   : 1;
} controller;

typedef struct globals {
  int first_mouse_reading;
  float dt;
} globals;

typedef struct camera {
  vec3 pos;
  vec3 front;
  vec3 up;

  float yaw;
  float pitch;
  vec3 direction;

  float speed;
  float fov;
} camera;

typedef struct cube {
  vec3 pos;
} cube;


camera cam;


controller ctrl;

globals global = {.first_mouse_reading = 1};

void process_keyboard(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);


float cube_model[720];

typedef struct timer {
  float start_time;
  float end_time;

  float to_render;
  float to_process;

} timer;

int main(void) {
  timer timer;

  cam = (camera){
    .speed  = 0.1f,
    .up     = {0.0f, 1.0f, 0.0f},
    .front  = {1.0f, 0.0f, 0.0f},
    .pos    = {-7.0f, 0.0f, 0.0f},
    .fov    = 3.1415f * 1.0f/5.0f
  };

  

  // Load our cube model
  FILE *ptr;
  ptr = fopen("data/cube.model","rb");
  int size;
  fread(&size,sizeof(int),1,ptr); // read how many floats in the model
  fread(cube_model,sizeof(float),size,ptr); // read that amount of floats to array
  fclose(ptr);


  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to make windows");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetCursorPosCallback(window, mouse_callback);  
  //glfwSetKeyCallback(window, keyboard_callback);  

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to start GLAD");
    return -1;
  }

  // GLFW-SETTINGS GO HERE
  
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

  // Lag shader
  int shader_program = createShader("shader.fs", "shader.vs");

  int texture = loadTexture("data/wall.jpg");
  
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
 
  glBindVertexArray(VAO);
 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_model), cube_model, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glfwMakeContextCurrent(window);


  glEnable(GL_DEPTH_TEST);


  mat4 proj  = {0};  // Brukes for å å lagre projeksjons-matrisen
  mat4 model = {0};  // Brukes for å lagre transformasjonen til modellen
  mat4 tmp   = {0};
 

  float aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

  float last_time    = 0.0f;
  float current_time = 0.0f;
  


  cube c1 = {.pos = {0.0f, 0.0f, -1.0f}};
  cube c2 = {.pos = {1.0f, 1.0f, 1.5f}};

  //glfwSwapInterval(0.0f);
  while (!glfwWindowShouldClose(window)) {

    
    current_time = glfwGetTime();
    global.dt    = current_time - last_time;
    last_time    = current_time;


    process_keyboard(window);
    

    // Rotate model transform

    /*
    float angle = 20.0f;
    clearM4(tmp);
    rotateZ(model, current_time*0.3f+angle, tmp);
    copyM4(tmp, model);
    rotateX(model, current_time+angle, tmp);
    copyM4(tmp, model);
    glBindTexture(GL_TEXTURE_2D, texture);
    */

    //
    // Render first cube
    //
    glUseProgram(shader_program);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setInt(shader_program, "texture0", 0);

    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(shader_program, "proj", proj);


    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(shader_program, "cam_pos",   cam.pos);
    setVec3(shader_program, "cam_front", cam.front);
    setVec3(shader_program, "cam_up",    cam.up);


    // Transler og tegn
    initIdM4(model);
    mkTranslation(model, c1.pos);
    setMat4(shader_program, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    initIdM4(model);
    mset(model, 0, 0, 0.5f);
    mset(model, 1, 1, 0.5f);
    mset(model, 2, 2, 0.5f);
    mset(model, 1, 1, 0.5f);
    mkTranslation(model, c2.pos);
    setMat4(shader_program, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);



    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
  return 0;
}




void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
  static float last_x;
  static float last_y;
  if (global.first_mouse_reading) {
    last_x = x_pos;
    last_y = y_pos;
    global.first_mouse_reading = 0;
  }

  float x_offset = x_pos - last_x;
  float y_offset = last_y - y_pos;
  last_x = x_pos;
  last_y = y_pos;

  const float sens = 0.1f;
  x_offset *= sens;
  y_offset *= sens;

  cam.yaw   += x_offset;
  cam.pitch += y_offset;


  if(cam.pitch > 89.0f)
    cam.pitch =  89.0f;
  if(cam.pitch < -89.0f)
    cam.pitch = -89.0f;

  float tmp_pitch = cam.pitch * 0.01745329251f;
  float tmp_yaw   = cam.yaw   * 0.01745329251f;

  
  cam.front[0] = cos(tmp_yaw) * cos(tmp_pitch);
  cam.front[1] = sin(tmp_pitch);
  cam.front[2] = sin(tmp_yaw) * cos(tmp_pitch);
  normalizeV3(cam.front);

  
}

void process_keyboard(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
  
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    cam.fov -= 0.01f;
  
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    cam.fov += 0.01f;
  
  if (cam.fov < 0.01f)
    cam.fov = 0.01f;


  float speed = 7.5f * global.dt;
   
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 tmp;
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 tmp;
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 tmp;
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 tmp;
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
   }

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    vec3 tmp;
    copyV3(cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
   }
  
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    vec3 tmp;
    copyV3(cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
   }
}
