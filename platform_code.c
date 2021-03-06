
//
// Om code-reloading
// https://www.danielecarbone.com/hot-code-reloading/
//

// https://www.glfw.org/docs/latest/compile.html#compile_manual
// https://lazyfoo.net/tutorials/SDL/index.php


#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef __AVX__
  #define STBI_NO_SIMD
#endif

#define  STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "linalg.h"
#include "helpers.h"
#include "shader.h"


const unsigned int SCR_WIDTH  = 1200;
const unsigned int SCR_HEIGHT = 800;


typedef enum Game_Mode {
  GAME,
  MENU,
} Game_Mode;

typedef struct Controller {
  u8 up;
  u8 down;
  u8 left;
  u8 right;

  u8 shift;

  u8 esc;
  u8 q;

  u8 throw;

} Controller;

typedef struct Globals {
  Game_Mode game_mode;
  float     dt;
  int       reset_mouse_pos; // Brukes for å unngå "hopping" i spillet eller når vi går inn/ut fra meny. Se for eksemel ESCAPE-delen av key_callback
  int       throw;
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
  vec3 speed;
} Cube;

typedef struct Stone {
  vec3  color;
  
  vec3  pos;
  vec3  speed;
  vec3  acc;

  vec3  tilt_angle;
  float spin_speed;

  float spin;

  int dead;
  int hops;
} Stone;

typedef struct Light_Cube {
  vec3 pos;
  vec3 color;
} Light_Cube;



//void process_keyboard(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void mouseButtonCallback(GLFWwindow* window, int key, int scancode, int);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);


Controller controller = {};
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

Globals global = {
  .reset_mouse_pos = 1,
  .game_mode = GAME,
  .dt = 0,
};

Camera cam = {
  .speed  = 0.1f,
  .up     = {0.0f, 1.0f, 0.0f},

  .front  = {0.16f, -0.10f, -0.98f}, // @OVERFLADISK .yaw og .pitch brukes til å regne
  .yaw    = -80.6f,               // ut .front hver frame, men for å starte i en
  .pitch  = -6.1f,                 // gitt retning må alle settes.......
  
  .pos    = {-1.0f, 2.0f, 6.0f},
  .fov    = 3.1415f * 1.0f/5.0f
};

Stone stone = {
  .pos   = {0.0f, 2.0f, 0.0f},
  .speed = {},
  .acc   = {},
  .color = {},
  .tilt_angle = {},
  .spin_speed = 0,
  .dead = 0,
};

Stone water = {
  .pos   = {0.0f, 0.0f, 0.0f},
  .color = {0.015f, 0.48f, 0.94f},
};

Stone init_stone = {
  .pos   = {-2.0f, 1.4f,   2.0f},
  .speed = {4.0f,  0.5f,  0.0f},
  .acc   = {0.0f,  -9.81f, 0.0f},
  
  .color = {.2f,  0.2f,   0.2f},
  
  .tilt_angle = {},
  .spin_speed = 12.0f,

  .hops = 0,
  .dead = 0,
};

Light_Cube light_cube = {
  .pos   = {20.0f, 8.0f, 20.0f},
  .color = {1.0f, 1.0f, 1.0f}
};



//void *memory[2*1024*1024];

int main(void) {

  //global.memory = malloc(4*1024*1024);
  
/*
  From LearnOpenGl:
  Since a vertex by itself has no surface (it's just a single point in space) we retrieve a normal vector by using its surrounding vertices to figure out the surface of the vertex. We can use a little trick to calculate the normal vectors for all the stone's vertices by using the cross product, but since a 3D stone is not a complicated shape we can simply manually add them to the vertex data.
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
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetKeyCallback(window, keyboardCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);


  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to start GLAD");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  
  //
  // Last inn shadere
  //
  
  Shader main_shader = {
    .name   = "main_shader",
    .f_path = "shaders/shader.fs",
    .v_path = "shaders/shader.vs",
  };

  Shader light_shader = {
    .name   = "light_shader",
    .f_path = "shaders/light_cube.fs",
    .v_path = "shaders/light_cube.vs",
  };
  
  if (!compileShader(&main_shader,  true)) exit(-1); // 1 betyr her at den skal printe debug data
  if (!compileShader(&light_shader, true)) exit(-1);

  

  //
  // Last inn tekstur
  //
  
  int texture = loadTexture("resources/wall.jpg");
  if (!texture) {
    return -1;
  }
   
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_model), cube_model, GL_STATIC_DRAW);


  unsigned int stoneVAO;
  glGenVertexArrays(1, &stoneVAO);
  glBindVertexArray(stoneVAO);
  
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
  
  int frame_count = 0;
  while (!glfwWindowShouldClose(window)) {
    if (frame_count % 60 == 0) {
      hotloadShader(&main_shader);
    }
    frame_count++;


    //
    // Calculate some time!
    //
    current_time = glfwGetTime();
    
    global.dt    = current_time - last_time;
    last_time    = current_time;

    //
    // Oppdater bevegelse
    //
    float speed = 7.5f * global.dt;
    float y_pos = cam.pos[1]; // Lagre z-posisjonen for å sette den igjen etterpå. Låser karakteren langs y-aksen.

    if (controller.shift) speed *= 4; 

    vec3 tmp; // Denne kan brukes uten å intialiseres siden vi alltid kopierer en verdi til den   
    if (controller.up) {
      copyV3(cam.front, tmp);
      scaleV3(tmp, speed);
      addV3(cam.pos, tmp);
    }
    
    if (controller.down) {
      copyV3(cam.front, tmp);
      scaleV3(tmp, speed);
      subV3(cam.pos, tmp);
    }
    
    if (controller.left) {
      crossV3(cam.front, cam.up, tmp);
      normalizeV3(tmp);
      scaleV3(tmp, speed);
      subV3(cam.pos, tmp);
    }

    if (controller.right) {
      crossV3(cam.front, cam.up, tmp);
      normalizeV3(tmp);
      scaleV3(tmp, speed);
      addV3(cam.pos, tmp);
    }

    cam.pos[1] = y_pos;







    
    if (controller.throw) {
      // @FORBEDRING - Basically må vi bestemme oss for hvordan vi skal gjennomføre kastet... Steinen skal ideelt sett ha en vinkel på 20 grader. Denne må være avhengig av hvor vi ser, men kanskje ikke direkte. Kanskje den kan være dobbelte av vår vinkel fra horisontalen... Vi får se!
      /*
        vec3 xz_dir = {cam.front[0], 0.0f, cam.front[2]}; // Isoler xz-komponenten
        normalizeV3(xz_dir); // @USIKKER tror man må normalisere??? Vet ikke
        float theta = angleV3(cam.front, xz_dir); // Finn vinkelen til kastet
        
        theta *= copysign(1.0f, cam.front[1]); // Gjenskap fortegn
        
        printf("%f\n", theta*180.0f/3.1415f);
      */
      
      // @OPPRYDDING - Dette må flyttes til simulasjons-koden.
      // @FORBEDRING - Vi vil at tiden man holder inne knappen skal øke kastets fart... Dette krever at vi lagrer tiden når knappen ble trykket, og når knappen slippes lager vi en delta som skaleres (logaritmisk?) slik at det skaleres om en toppverdi.
      
      stone = init_stone;
      stone.pos[0] = cam.pos[0]; // Kopier bare x- og z-koordinater, høyden vil være fast i 'kaste-høyde'
      stone.pos[2] = cam.pos[2]; //

      addV3(stone.pos, cam.front); // @FORBEDRING - Dette var et forsøk på å flytte den nye stenen lengre fram... Det vi vil er at stenen skal holdes 'foran' oss, og at kamera vender bakover mens man lader opp til kast... Får se mer på dette senere :)

      copyV3(cam.front, stone.speed);
      scaleV3(stone.speed, 14.0f);
      controller.throw = 0;
    }
       
    //
    // Simulate stone
    //

    if (!stone.dead) {
      // Legg til akselerasjon til farten
      vec3 tmp_acc;
      copyV3(stone.acc, tmp_acc);
      scaleV3(tmp_acc, global.dt);
      addV3(stone.speed, tmp_acc);
      
      // Legg til summen av fart til posisjon
      vec3 tmp_speed;
      copyV3(stone.speed, tmp_speed); 
      scaleV3(tmp_speed, global.dt);
      addV3(stone.pos, tmp_speed);
      
      
      // Rotér steinen med nåværende spin_speed
      stone.spin += stone.spin_speed * global.dt;
      
      
      // @FORBEDRING - ønsker å legge til friksjon langs horsintalen også... Dette kan skje både i luften (luftmotsatnd) og ved kontakt med vannet her..
      
      if (stone.pos[1] < 0) {
        stone.hops  += 1;
        stone.pos[1] = 0;
        
        // Friksjonen avhenger ved vinkelen steinen treffer vannet ved, i forhold til horisontalen. Denne går mellom 0 og 1.56, og vi vil skalerer disse verdiene til å gå mellom 1 og 0, hvor 1 bevarer all fart og 0 ikke bevarer noe fart. -bl 14.02.2020
        
        // @FORBEDRING! Friksjon regnes ut på en dum måte nå. Vinkelen på kastet er nå samme som vinkelen kameraet ser med horisontalen. Dette er greit. Friksjonen er derimot avhengig av denne vinkelen, som ikke stemmer helt... Vikipedia sier at den optimale vinkelen skal være 20 grader, altså 0.349 radianer. Vi må sette denne til å gi størst bevaring (0.8 / 0.9 eller noe), og la bevaringen gå ned på noe vis med høyere og lavere vinkler.

        // @MATTE noe er feil med utregningen av denne vinkelen... den skal liksom svare til hvor mange grader opp fra xz-planet den er, men det er den ikke... Kan være noe feil i 'angleV3'-funksjonen.... Hurra for bugs!

        // @USIKKER vi snur steinens fart FØR vi sjekker vinkel :D
        stone.speed[1] = -stone.speed[1];
        
        vec3 xz_dir = {stone.speed[0], 0.0f, stone.speed[2]}; // Isoler xz-komponenten
        normalizeV3(xz_dir); // @USIKKER tror man må normalisere??? Vet ikke
        float theta = angleV3(stone.speed, xz_dir); // Finn vinkelen til kastet langs xz-komponenten

        //printf("deg: %f\n", theta * 180 / 3.1415);
        //printf("fri: %f\n", fabs(theta-0.349066f)*4.0f);
        // float theta = -atan(stone.speed[1]/stone.speed[0]) / 1.56f; // Den gamle måten å regne ut vinkel.. Tror denne stemte?

        float friction = (1.0f - min(fabs(0.349-theta)*5.0f, 1.0f))*0.9f;
              
        // Flipp y-retning (siden vi har truffet vannet, og møter "uendelig stor masse".
        
        scaleV3(stone.speed, friction); // Skaler hele farten med friksjonen!!!
        stone.spin_speed *= friction;   // Reduser også spinne-hastigheten
      }
     
      // Når farten er liten nok bare slutter vi å simuler steinen.
      if (lenV3(stone.speed) < 0.5f) {
        stone.dead = 1;
        //printf("Hops: %i\n", stone.hops);
      }
    }
    
    
    
    //
    // Render first stone
    //
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glUseProgram(main_shader.id);
    glBindVertexArray(stoneVAO);
    
    //setInt(shader_program, "texture0", 0);
    setVec3(main_shader.id, "view_color",   cam.pos);
    setVec3(main_shader.id, "object_color", stone.color);
    setVec3(main_shader.id, "light_color",  light_cube.color);
    setVec3(main_shader.id, "light_pos",    light_cube.pos);    
    
    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(main_shader.id, "proj", proj);

    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(main_shader.id, "cam_pos",   cam.pos);
    setVec3(main_shader.id, "cam_front", cam.front);
    setVec3(main_shader.id, "cam_up",    cam.up);

    // Transler og tegn første kube


    // @TANKER - rekkefølge for skalering
    // DONE - Skaler modell
    // DONE - Roter langs y-akse med tid
    // TODO - Vend bakover mot utkaststed.... må tenke litt på dette, må nok gjøres med en generell rotasjonsmatrise: https://en.wikipedia.org/wiki/Rotation_matrix#Nested_dimensions (ser ut som GPU-arbeid :) )

    clearM4(model);
   
    mat4 scale;
    clearM4(scale);
    initIdM4(scale);
    mset(scale, 0, 0, 0.5f);
    mset(scale, 1, 1, 0.1f);
    mset(scale, 2, 2, 0.5f);

    rotateY(scale, stone.spin, model); // Roter 'scale' med 'stone.spin' radianer om y-aksen, og lagre resultatet i 'model'
    mkTranslation(model, stone.pos);   // Transler 'model' til stone.pos

    setMat4(main_shader.id, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);



    //
    // Tegn 'vannet'
    //
    
    //setInt(shader_program, "texture0", 0);
    setVec3(main_shader.id, "view_color",   cam.pos);
    setVec3(main_shader.id, "object_color", water.color);
    setVec3(main_shader.id, "light_color",  light_cube.color);
    setVec3(main_shader.id, "light_pos",    light_cube.pos);    
    
    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(main_shader.id, "proj", proj);

    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(main_shader.id, "cam_pos",   cam.pos);
    setVec3(main_shader.id, "cam_front", cam.front);
    setVec3(main_shader.id, "cam_up",    cam.up);

    clearM4(model);
    initIdM4(model);
    clearM4(scale);
    initIdM4(scale);
    mset(model, 0, 0, 1000.5f);
    mset(model, 1, 1, 0.005f);
    mset(model, 2, 2, 1000.0f);

    mkTranslation(model, water.pos);   // Transler 'model' til stone.pos

    setMat4(main_shader.id, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);



    
    //
    // Tegn lys-kube
    //
    
    glUseProgram(light_shader.id);
    glBindVertexArray(lightVAO);

    setVec3(light_shader.id, "light_color",  light_cube.color);
    
    
    // Calculate perspective
    perspective(cam.fov, aspect_ratio, 0.1f, 100.0f, proj);
    setMat4(light_shader.id, "proj", proj);

    // Load in the vectors that are used to make the lookAt-matrix
    setVec3(light_shader.id, "cam_pos",   cam.pos);
    setVec3(light_shader.id, "cam_front", cam.front);
    setVec3(light_shader.id, "cam_up",    cam.up);
    // Transler og tegn andre kube. Denne skaleres også ned med matrisen under.
    clearM4(model);
    initIdM4(model);
    mset(model, 0, 0, 0.5f);
    mset(model, 1, 1, 0.5f);
    mset(model, 2, 2, 0.5f);
        
    mkTranslation(model, light_cube.pos);
    
    setMat4(light_shader.id, "model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();

    
  }
  
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
  //return 0;
}



void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double x_pos, double y_pos) {

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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    controller.throw = 1;
    //popup_menu();
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);


    
    // @URYDDIG liker ikke at dette er gjort i en callback..    
    // ... Vil heller gjøre dette i hoved-loopen inntil videre, slik at vi ikke sprer viktig kode for mye utover. Kan kanskje sette noen globale variabler som leses.
    if (key ==  GLFW_KEY_M && action == GLFW_PRESS) {
      if (global.game_mode == GAME) {
        global.game_mode = MENU;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      } else if (global.game_mode == MENU) {
        global.game_mode = GAME;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        global.reset_mouse_pos = 1;
      }
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      controller.throw = 1;
    }
    
    
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
      cam.fov -= 0.01f;
    
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
      cam.fov += 0.01f;
    
  if (cam.fov < 0.01f)
    cam.fov = 0.01f;    
  
  if (key == GLFW_KEY_W) controller.up    = action; // 0 for release, 1 for press
  if (key == GLFW_KEY_S) controller.down  = action; // 0 for release, 1 for press
  if (key == GLFW_KEY_D) controller.right = action; // 0 for release, 1 for press
  if (key == GLFW_KEY_A) controller.left  = action; // 0 for release, 1 for press

  controller.shift = mods & GLFW_MOD_SHIFT;
    

}
