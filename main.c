#include <stdio.h>
#include <glad/glad.h>
#include <SDL2/SDL.h>
//include <SDL2/SDL_opengl.h>

#include <math.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "linalg.h"
#include "helpers.h"

#define internal        static
#define local_persist   static
#define global_variable static

#define SCREEN_WIDTH  600
#define SCREEN_HEIGHT 480

#define using_entity int pid; int pos;
typedef struct entity {
  using_entity
} entity;

typedef struct guy {
  using_entity
} guy;

typedef struct camera {
  vec3 pos;
  vec3 front;
  vec3 target;
  vec3 up;
  float speed;
  float fov;
} camera;

camera cam;

 
int main() {
  SDL_Window *window;
  SDL_GLContext context;

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


  window = SDL_CreateWindow( "GARDEN",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH, SCREEN_HEIGHT,
                             SDL_WINDOW_OPENGL);
  if (!window) {
    printf("failed to make window!\n\t%s", SDL_GetError());
    return 1;
  }
  context = SDL_GL_CreateContext(window);
  if (!context) {
    printf("failed to make context!\n\t%s", SDL_GetError());
    return 1;
  }

  // Fra micrui
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glDisable(GL_CULL_FACE);
  //glDisable(GL_DEPTH_TEST);
  //glEnable(GL_SCISSOR_TEST);
  //glEnable(GL_TEXTURE_2D);
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);

  /*
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, ATLAS_WIDTH, ATLAS_HEIGHT, 0,
    GL_ALPHA, GL_UNSIGNED_BYTE, atlas_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  assert(glGetError() == 0);
  */


 

  //
  // Lag window og gl-context
  //
 
  if (SDL_GL_MakeCurrent(window, context)) {
    // 0 on success, negative on failure
    printf("failed to set context!\n\t%s", SDL_GetError());
    return 1;
  }
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    printf("failed to get proc loader :/");
    return -1;
  }
  if (!gladLoadGL()) {
    printf("failed to load gl with glad :/");
    return -1;
  }
  SDL_GL_SetSwapInterval(1);



  //
  // Kjør kode!
  //
  
  int shader_program  = glCreateProgram();
  int fragment_shader = loadShader("shader.fs", GL_FRAGMENT_SHADER);
  int vertex_shader   = loadShader("shader.vs", GL_VERTEX_SHADER);

  glAttachShader(shader_program, fragment_shader);
  glAttachShader(shader_program, vertex_shader);
  glLinkProgram(shader_program);

  // check for linking errors
  {
    int success; char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_program, 512, NULL, info_log);
      printf("Failed to link shader\n\t%s", info_log);
    }
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  //
  // last inn teksturer og modeller
  //
  float cube_model[720];
  {
    // Load our cube model
    FILE *ptr;
    ptr = fopen("data/cube.model","rb");
    int size;
    fread(&size,sizeof(int),1,ptr); // read how many floats in the model
    fread(cube_model,sizeof(float),size,ptr); // read that amount of floats to array
    fclose(ptr);
  }

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
  //lfwMakeContextCurrent(window);

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
  //
  // forbered mer data
  //
  
  vec3 *cubes = (vec3 *)cubois;

  mat4 proj;   // Used for storing the projection matrix
  mat4 model;  // Used for storing the model transform to be applied
  mat4 tmp;
 
  cam = (camera){
    .speed  = 0.1f,
    .up     = {0.0f, 1.0f, 0.0f},
    .target = {0.0f, 0.0f, 0.0f},
    .front  = {0.0f, 0.0f, -1.0f},
    .pos    = {0.0f, 0.0f, 8.0f},
    .fov    = 3.1415f * 1.0f/5.0f
  };

  float aspect_ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;


  //
  // render!
  //

  float kill_time = 2.0f;
  float kill_start = 0.001f*(float)SDL_GetTicks(); //glfwGetTime();
  int quit = 1;
  while (quit) { // @uferdig legg til sjekk her for å krysse ut
    //processInput(window);

    double xpos, ypos;
    xpos = ypos = 0;
    //glfwGetCursorPos(window, &xpos, &ypos);
   
    float time = 0.001f*(float)SDL_GetTicks();
    if (time - kill_start > kill_time) {
      quit = 0;
    }
   
    xpos /= SCREEN_WIDTH;
    ypos /= SCREEN_HEIGHT;

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

    for(unsigned int i = 0; i < 10; i++) {
      initIdM4(model);
      float angle = 20.0f * i;

      clearM4(tmp);
      rotateZ(model, time*0.3f+angle, tmp);
      copyM4(tmp, model);
      rotateX(model, time+angle, tmp);
      copyM4(tmp, model);
      glBindTexture(GL_TEXTURE_2D, texture);

      vec3 translation;
      copyV3(cubes[i], translation);
      
      mkTranslation(model, translation);
      setMat4(shader_program, "model", model);
        
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

  SDL_GL_SwapWindow(window);

  }
  
  //
  // Rydd opp
  //
  
  SDL_GL_DeleteContext(context);
  SDL_Quit();

                                  
  return 0;
}

