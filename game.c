

// https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideoopengl.html
// https://wiki.libsdl.org/SDL_GL_CreateContext
// https://www.glfw.org/docs/latest/compile.html#compile_manual
// https://lazyfoo.net/tutorials/SDL/index.php

#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)
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

camera cam;
controller ctrl;

globals global = {.first_mouse_reading = 1};


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
  printV3(cam.front);

  
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
}

float cube_model[720];

typedef struct timer {
  float start_time;
  float end_time;

  float to_render;
  float to_process;

} timer;

int main(void) {
  timer timer;


  /*
  FILE *write_ptr;
  write_ptr = fopen("cube.model","wb");  // w for write, b for binary
  int size = sizeof(vertices);
  int fun = 69;
  fwrite(&size,sizeof(int),1,write_ptr); // write 10 bytes from our buffe
  fwrite(vertices,size,1,write_ptr); // write 10 bytes from our buffe
  //fwrite(vertices,sizeof(vertices),1,write_ptr); // write 10 bytes from our buffe
  fclose(write_ptr);
  */
  {
    // Load our cube model
    FILE *ptr;
    ptr = fopen("data/cube.model","rb");
    int size;
    fread(&size,sizeof(int),1,ptr); // read how many floats in the model
    fread(cube_model,sizeof(float),size,ptr); // read that amount of floats to array
    fclose(ptr);
  }


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

  int fragment_shader = loadShader("shader.fs", GL_FRAGMENT_SHADER);
  int vertex_shader   = loadShader("shader.vs", GL_VERTEX_SHADER);

  // Link shadere til et shader objekt, og slett shaderene
  int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  // check for linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("Failed to link fragment shader");
    printf("%s", infoLog);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  //
  // Load texture file
  //
  glActiveTexture(GL_TEXTURE0);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  int width, height, nrChannels;
  unsigned char *data = stbi_load("data/wall.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Klarte ikke å laste inn bildet!\n");
  }


  stbi_image_free(data);
  
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

  // glfwSetKeyCallback(window, key_callback);


  glEnable(GL_DEPTH_TEST);

  float cubois[] = {
    0.0f,  0.0f,  0.0f, 
    2.0f,  5.0f, -15.0f, 
    -1.5f, -2.2f, -2.5f,  
    -3.8f, -2.0f, -12.3f,  
    2.4f, -0.4f, -3.5f,  
    -1.7f,  3.0f, -7.5f,  
    1.3f, -2.0f, -2.5f,  
    1.5f,  2.0f, -2.5f, 
    1.5f,  0.2f, -1.5f, 
    -1.3f,  1.0f, -1.5f 
  };

  vec3 *cubes = (vec3 *)cubois;

  mat4 proj;   // Used for storing the projection matrix
  mat4 model;  // Used for storing the model transform to be applied
  mat4 tmp;
 
  cam = (camera){
    .speed  = 0.1f,
    .up     = {0.0f, 1.0f, 0.0f},
    .front  = {1.0f, 0.0f, 0.0f},
    .pos    = {-15.0f, 1.0f, -20.0f},
    .fov    = 3.1415f * 1.0f/5.0f
  };

  float aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

  float last_time    = 0.0f;
  float current_time = 0.0f;
  
  //glfwSwapInterval(0.0f);
  while (!glfwWindowShouldClose(window)) {

    current_time = glfwGetTime();
    global.dt = current_time - last_time;
    last_time = current_time;


    process_keyboard(window);
    


   
    perspective(cam.fov, aspect_ratio,
                0.1f, 100.0f, proj);
   
    // Load into uniform
    glUseProgram(shader_program);
    setInt(shader_program,  "texture0", 0);
    setMat4(shader_program, "proj", proj);

    setVec3(shader_program, "cam_pos",     cam.pos);
    setVec3(shader_program, "cam_front",   cam.front);
    setVec3(shader_program, "cam_up",      cam.up);

    // Render
    glUseProgram(shader_program);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);

    for(unsigned int i = 0; i < 8; i++) {
      initIdM4(model);
      float angle = 20.0f * i;

      clearM4(tmp);
      rotateZ(model, current_time*0.3f+angle, tmp);
      copyM4(tmp, model);
      rotateX(model, current_time+angle, tmp);
      copyM4(tmp, model);
      glBindTexture(GL_TEXTURE_2D, texture);

      vec3 translation;
      copyV3(cubes[i], translation);
      
      mkTranslation(model, translation);
      setMat4(shader_program, "model", model);
        
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    printV3(cam.pos);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
  return 0;
}
