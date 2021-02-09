

// https://www.glfw.org/docs/latest/compile.html#compile_manual
// https://lazyfoo.net/tutorials/SDL/index.php

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <time.h>

#include <stdint.h>

#define s8  int8_t
#define u8  uint8_t
#define s16 int16_t
#define u16 uint16_t
#define s32 int32_t
#define u32 uint32_t


#ifndef __AVX__
  #define STBI_NO_SIMD
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "linalg.h"
#include "helpers.h"
#include <unistd.h>




const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

typedef enum Game_Mode {
  GAME,
  MENU,
} Game_Mode;

typedef struct Controller {
  u8 up;
  u8 down;
  u8 left;
  u8 right;

  u8 esc;
  u8 q;
} Controller;

typedef struct Globals {
  Game_Mode game_mode;
  float     dt;
  int       reset_mouse_pos; // Brukes for å unngå "hopping" i spillet eller når vi går inn/ut fra meny. Se for eksemel ESCAPE-delen av key_callback
} Globals;

typedef struct Camera {
  vec3 pos;
  vec3 front;
  vec3 up;

  float yaw;
  float pitch;
  vec3  direction;

  float speed;
  float fov;
} Camera;

typedef struct Cube {
  vec3 pos;
  vec3 color;
} Cube;

typedef struct Light_Cube {
  vec3 pos;
  vec3 color;
} Light_Cube;



void process_keyboard(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);



// controller ctrl = {};
float cube_model[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

//float cube_model[720];

Camera cam = {
  .speed  = 0.1f,
  .up     = {0.0f, 1.0f, 0.0f},

  .front  = {1.0f, 0.0f, 0.0f},
  .pos    = {-7.0f, 0.0f, 0.0f},
  .fov    = 3.1415f * 1.0f/5.0f
};


Globals global = {
  .reset_mouse_pos = 1,
  .game_mode = GAME,
  .dt = 0,
};


Cube cube = {
  .pos   = {0.0f, 0.0f, -1.0f},
  .color = {1.0f, 0.5f, 0.31f}
};

Light_Cube light_cube = {
  .pos   = {1.0f, 1.0f, 1.5f},
  .color = {1.0f, 1.0f, 1.0f}
};
  


int main(void) {

  // Load our cube model
  /*
  FILE *ptr;
  ptr = fopen("data/cube.model","rb");
  int size;
  fread(&size,sizeof(int),1,ptr); // read how many floats in the model
  fread(cube_model,sizeof(float),size,ptr); // read that amount of floats to array
  fclose(ptr);
  */
  
  
/*
  From LearnOpenGl:
  Since a vertex by itself has no surface (it's just a single point in space) we retrieve a normal vector by using its surrounding vertices to figure out the surface of the vertex. We can use a little trick to calculate the normal vectors for all the cube's vertices by using the cross product, but since a 3D cube is not a complicated shape we can simply manually add them to the vertex data.
*/
    
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GARDEN", NULL, NULL);
  if (window == NULL) {
    printf("Failed to make windows");
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);
 
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to start GLAD");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);


  // Lag shader
  int shader_program = createShader("shaders/shader.fs",     "shaders/shader.vs");
  int lc_shader      = createShader("shaders/light_cube.fs", "shaders/light_cube.vs");

  if (!shader_program || !lc_shader) {
    return -1;
  }
  
  int texture = loadTexture("resources/wall.jpg");
  if (!texture) {
    return -1;
  }
   
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_model), cube_model, GL_STATIC_DRAW);


  unsigned int cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glBindVertexArray(cubeVAO);
  
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  
  unsigned int lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  
  // Posisjonsattributt
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  
  // These can either be initialized here or cleared before use in the render code, but skipping the cleaning will lead to crashes!
  mat4 proj  = {};  // Brukes for å å lagre projeksjons-matrisen
  mat4 model = {};  // Brukes for å lagre transformasjonen til modellen
  mat4 tmp   = {};
 

  float aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

  float last_time    = 0.0f;
  float current_time = 0.0f;
  

  //glfwSwapInterval(0.0f);
  while (!glfwWindowShouldClose(window)) {

    //
    // Calculate some time!
    current_time = glfwGetTime();

    global.dt    = current_time - last_time;
    last_time    = current_time;
    
    //
    // Render first cube
    //

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glUseProgram(shader_program);
    glBindVertexArray(cubeVAO);

    //setInt(shader_program, "texture0", 0);
    setVec3(shader_program, "view_color",   cam.pos);
    setVec3(shader_program, "object_color", cube.color);
    setVec3(shader_program, "light_color",  light_cube.color);
    setVec3(shader_program, "light_pos",    light_cube.pos);    
    
    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(shader_program, "proj", proj);

    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(shader_program, "cam_pos",   cam.pos);
    setVec3(shader_program, "cam_front", cam.front);
    setVec3(shader_program, "cam_up",    cam.up);

    // Transler og tegn første kube
    initIdM4(model);

    rotateZ(model, current_time*0.3f, tmp);
    copyM4(tmp, model);
    mkTranslation(model, cube.pos);
    setMat4(shader_program, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    //
    // Tegn lys-kube
    glUseProgram(lc_shader);
    glBindVertexArray(lightVAO);

    setVec3(lc_shader, "light_color",  light_cube.color);
    
    
    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(lc_shader, "proj", proj);

    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(lc_shader, "cam_pos",   cam.pos);
    setVec3(lc_shader, "cam_front", cam.front);
    setVec3(lc_shader, "cam_up",    cam.up);
    // Transler og tegn andre kube. Denne skaleres også ned med matrisen under.
    clearM4(model);
    initIdM4(model);
    mset(model, 0, 0, 0.5f);
    mset(model, 1, 1, 0.5f);
    mset(model, 2, 2, 0.5f);
    mset(model, 1, 1, 0.5f);
    
    mkTranslation(model, light_cube.pos);
    
    setMat4(lc_shader, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
  //return 0;
}




void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {

  if (global.game_mode == MENU) {
    // We do not want game movement in the menu...
    return;
  }

  static float last_x;
  static float last_y;
  if (global.reset_mouse_pos) {
    last_x = x_pos;
    last_y = y_pos;
    global.reset_mouse_pos = 0;
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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);

    // @URYDDIG liker ikke at dette er gjort i en callback..    
    // ... Vil heller gjøre dette i hoved-loopen inntil videre, slik at vi ikke sprer viktig kode for mye utover. Kan kanskje sette noen globale variabler som leses.
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
      if (global.game_mode == GAME) {
        global.game_mode = MENU;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      } else if (global.game_mode == MENU) {
        global.game_mode = GAME;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        global.reset_mouse_pos = 1;
      }
    }
}


void process_keyboard(GLFWwindow *window) {
  assert(0); // We do not want to end up here.

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if ((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)) {
    if (global.game_mode == GAME) {
      printf("Vi er nå i menyen!\n");
      global.game_mode = MENU;
    }
    if (global.game_mode == MENU) {
      printf("Vi er nå i spillet!\n");
      global.game_mode = GAME;
    }
    //global.game_mode_changed = 1;
  }
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
    //printf("Kommer vi hit?\n");
    //global.game_mode_changed = 0;
  }
                                                           
  if (global.game_mode == MENU) {
    return;
  }
  
  
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    cam.fov -= 0.01f;
  
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    cam.fov += 0.01f;
  
  if (cam.fov < 0.01f)
    cam.fov = 0.01f;


  float speed = 7.5f * global.dt;
  float y_pos = cam.pos[1]; // Lagre z-posisjonen for å sette den igjen etterpå. Låser karakteren langs y-aksen.


  vec3 tmp; // Denne kan brukes uten å intialiseres siden vi alltid kopierer en verdi til den   
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
   }

  cam.pos[1] = y_pos;

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    copyV3(cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    addV3(cam.pos, tmp);
   }
  
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    copyV3(cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    subV3(cam.pos, tmp);
   }



  // Bevegelser for kuben
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    addV3(cube.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    copyV3(cam.front, tmp);
    scaleV3(tmp, speed);
    subV3(cube.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    subV3(cube.pos, tmp);
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    crossV3(cam.front, cam.up, tmp);
    normalizeV3(tmp);
    scaleV3(tmp, speed);
    addV3(cube.pos, tmp);
   }
  
}
