/*
  OK! Så det er en del ting som må gjøres her.
  Det viktigste er kanskje å holde styr på hva som skal læres. Målet er å komme gjennom 'learnopengl'-leksjonene...

  Så:
  SDL input. 
    Dette burde ikke være så vanskelig. Har kode tilgjengelig i microui-koden som kan kokes.
  Microui. 
    Kan eventuelt se om dear imgui er lettere å bruke. Dette vil gjøre det lettere å teste enkle variasjoner i kode
  Faktorisering.
   Dette er en stor oppgave. Her vil jeg se tilbake på HHH, og se over hvordan Casey løser code reloading osv..
    Men dette kan la seg vente litt, det er ikke kritisk. Burde kanskje la arkitekturen være noe flytene, og leke
    litt med hvordan ting kan organiseres! Bygge smak =)

  Men husk Børge, dette er for å lære, det skal ikke være så serriøst! Så ta det med ro og kos deg!
 */


#include <stdio.h>
#include <limits.h>
#include <glad/glad.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

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

typedef struct camera {
  vec3 pos;
  vec3 front;
  vec3 target;
  vec3 up;
  float speed;
  float fov;
} camera;

typedef struct keyboard {
  int up    : 1;
  int down  : 1;
  int left  : 1;
  int right : 1;
  int one   : 1;
  int two   : 1;
} keyboard;

camera   cam;
keyboard kb = {0};

static SDL_Window *window;
static SDL_GLContext context;

void print_kb(keyboard kb)
{
  printf("up: %i\ndown: %i\nleft: %i\nright: %i\none: %i\ntwo: %i\n\n", kb.up, kb.down, kb.left, kb.right, kb.one, kb.two);
}

int main() {

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


 

  //
  // Lag vindu og gl-kontekst
  // Her setter vi først GL til å være vår kontekst. Når dette er gjort kan gl-funksjoner lastes inn.
  // DERETTER kan vi begynne å sette ulike egenenskaper for GL med glEnable!
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

  //
  // Noen av disse er hentet fra microui-biblioteket..
  // @todo les deg opp på hva de gjør!
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_TEXTURE_2D);

  glEnable(GL_DEPTH_TEST);
  SDL_GL_SetSwapInterval(1);



  
  //
  // Last inn shadere
  //
  
  int shader_program  = glCreateProgram();
  int fragment_shader = loadShader("shaders/fragment.gl", GL_FRAGMENT_SHADER);
  int vertex_shader   = loadShader("shaders/vertex.gl", GL_VERTEX_SHADER);

  glAttachShader(shader_program, fragment_shader);
  glAttachShader(shader_program, vertex_shader);
  glLinkProgram(shader_program);

  // Sjekk for feil ved linking
  int success; char info_log[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, info_log);
    printf("Failed to link shader\n\t%s", info_log);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  //
  // Last inn modeller og teksturer
  // Her laster vi inn kube-modellen og murvegg-teksturen.
  //
  // Merk at det ser ut til at modellen må ligge _her_. Dette er mest sannsynlig en bug? Minnet burde jo kunne ligge hvorsomhelst...
  // For eksempel som static utenfor main, eller allokert et sted. Dette må forskes på!
  float cube_model[1024];
  FILE *ptr;
  ptr = fopen("data/cube.model","rb");
  int size;
  fread(&size,sizeof(int),1,ptr);           // read how many floats in the model
  fread(cube_model,sizeof(float),size,ptr); // read that amount of floats to array
  fclose(ptr);

  //
  // Last inn den eminente tekstur
  // @rydding dette kan vi kanskje trekke ut til en funksjon, siden det er en del jobb...
  
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
    printf("Klarte ikke å laste inn tekstur!!\n");
  }

  stbi_image_free(data);
  
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
 
  glBindVertexArray(VAO);
 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_model), cube_model, GL_STATIC_DRAW);

  // Legg inn posisjons-attributt
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Legg inn tekstur-attributt
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  //
  // Her har vi noe data.. Dette kan flyttes til entities i nær framtid når vi skal forsøke å refactore koden til ulike deler...

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

 
  vec3 *cubes = (vec3 *)cubois; // Her tolker vi cubois som en array med vec3'er i stedet :)

  mat4 proj;   // Her lagres projeksjonsmatrisen
  mat4 model;  // Her lagres model-transformasjonen som gjøres
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
  // Dette er rendering-biten. Den er nogså uryddig, ettersom at den innebærer noen biter som hører til simulering heller enn rendering.

  float kill_time = 2.0f;
  float kill_start = 0.001f*(float)SDL_GetTicks(); //glfwGetTime();
  int quit = 0;
  while (!quit) { // @uferdig legg til sjekk her for å krysse ut
    //processInput(window);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT: quit = 0; break;
          //case SDL_MOUSEMOTION: mu_input_mousemove(ctx, e.motion.x, e.motion.y); break;
          //case SDL_MOUSEWHEEL: mu_input_scroll(ctx, 0, e.wheel.y * -30); break;
          //case SDL_TEXTINPUT: mu_input_text(ctx, e.text.text); break;
        /*
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        int b = button_map[e.button.button & 0xff];
        if (b && e.type == SDL_MOUSEBUTTONDOWN) { mu_input_mousedown(ctx, e.button.x, e.button.y, b); }
        if (b && e.type ==   SDL_MOUSEBUTTONUP) { mu_input_mouseup(ctx, e.button.x, e.button.y, b);   }
          break;
        }
        */

      case SDL_KEYDOWN:
          switch( e.key.keysym.sym ){
          case SDLK_1:
            kb.one = 1;
            break;
          case SDLK_2:
            kb.two = 1;
            break;
          case SDLK_w:
            kb.up = 1;
            break;
          case SDLK_s:
            kb.down = 1;
            break;
          case SDLK_a:
            kb.left = 1;
            break;
          case SDLK_d:
            kb.right = 1;
            break;
          default:
            break;
        }
      case SDL_KEYUP: {
        if (e.type == SDL_KEYUP) {
          switch( e.key.keysym.sym ){
          case SDLK_ESCAPE:
            quit = 1;
            break;
          case SDLK_1:
            kb.one = 0;
            break;
          case SDLK_2:
            kb.two = 0;
            break;
          case SDLK_w:
            kb.up = 0;
            break;
          case SDLK_s:
            kb.down = 0;
            break;
          case SDLK_a:
            kb.left = 0;
            break;
          case SDLK_d:
            kb.right = 0;
            break;
          default:
            break;
          }
        }
        }
      }
    }
    print_kb(kb);

    if (kb.one) {
      cam.fov += 0.010f;
    }
    if (kb.two) {
      cam.fov -= 0.010f;
    }

    double xpos, ypos;
    xpos = ypos = 0;
    //glfwGetCursorPos(window, &xpos, &ypos);
   
    float time = 0.001f*(float)SDL_GetTicks();
    //if (time - kill_start > kill_time) {
    //  quit = 1;
    // }
   
    xpos /= SCREEN_WIDTH;
    ypos /= SCREEN_HEIGHT;

    perspective(cam.fov, aspect_ratio, // @refactor denne beregningen kan kanskje flyttes til gpu?
                0.1f, 100.0f, proj);
   
    // Last konstanter inn til shaderen!
    glUseProgram(shader_program);
    setInt(shader_program,  "texture0", 0);
    setMat4(shader_program, "proj", proj);

    setVec3(shader_program, "cam_pos",     cam.pos);
    setVec3(shader_program, "cam_front",   cam.front);
    setVec3(shader_program, "cam_up",      cam.up);

    //
    // Faktisk utfør rendering
    // Siden vi her er veldig kjent med hva som skal rendres er dette lett, men det vil bli mer komplekst senere..
    // Når vi flytter rendering ut av denne koden blir det moro!
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
  // Her skal dette være nok å gjøre?
  
  SDL_GL_DeleteContext(context);
  SDL_Quit();

                                  
  return 0;
}

