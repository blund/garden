
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BL_IMPL
#include "bl.h"

#define READFILE_IMPL
#include "read_file.h"

#define LINALG_IMPL
#include "linalg.h"

#include "things/thing.h"
#include "things/land.h"
#include "things/light.h"
#include "things/waves.h"
#include "things/raycast.h"

#include "state.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void process_input(GLFWwindow *window);

int screen_width = 800;
int screen_height = 600;

vec3 fps_pos = {0.0f, 0.8f, -1.0f}; // 1 unit above ground
vec3 fps_front = {0.0f, 0.0f, -1.0f}; // looking forward on Z
vec3 fps_up = {0.0f, 1.0f, 0.0f};     // global up direction

float camera_angle = 0;

float last_mouse_x = 400, last_mouse_y = 300;
float yaw = -90.0f; // left/right
float pitch = 0.0f; // up/down

int firstMouse = 1;

thing ray_thing;
typedef struct {
  float start_time;
  vec3  origin;
} ripple;

typedef struct {
  int index;
  int size;
  ripple ripples[8];
} ripple_controller;

ripple_controller rc = {0,8};


global_state state;

int main() {
  // load glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);  
 
  // set up window
  GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Welcome to the Garden", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "failed to create glfw window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  
  // set initial viewport
  glViewport(0, 0, 800, 600);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // set framebuffer resize callback :)
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  thing waves;
  compile_shader(&waves, "shaders/waves.vs", "shaders/waves.fs");
  create_waves(&waves);

  thing land;
  compile_shader(&land, "shaders/land.vs", "shaders/land.fs");
  create_land(&land);

  thing light;
  compile_shader(&light, "shaders/light.vs", "shaders/light.fs");
  create_light(&light);

  compile_shader(&ray_thing, "shaders/raycast.vs", "shaders/raycast.fs");
  create_raycast(&ray_thing);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    //mat4 projection;
    float aspect = 800.0f / 600.0f; // use your real window size
    mat4_perspective(45.0f, aspect, 0.1f, 100.0f, state.proj);

    //mat4 view;
    vec3 center;
    vec3_add(fps_pos, fps_front, center);
    mat4_lookat(fps_pos, center, fps_up, state.view);

    mat4_identity(state.model);
    //mat4_scale(state.model, 10.0f, 4.0f, 10.0f);
    
    state.time = (float)glfwGetTime();

    render_light(&state, &light);
    render_land(&state, &land);
    render_waves(&state, &waves, rc.ripples[0].origin,
                 rc.ripples[0].start_time);
    render_raycast(&state, &ray_thing);
 
    // finish render!
    glfwSwapBuffers(window);
    glfwPollEvents();    
  }

  // terminate program
  glfwTerminate();
  printf("Nothing's really happened like I thought it would\n");
  return 0;
}

// on resize, simply set new bounds
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, width, height);
}


void process_input(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }

  float speed = 0.03f;

    vec3 right, up = {0.0f, 1.0f, 0.0f};
    vec3_cross(fps_front, up, right);
    vec3_normalize(right, right);
   
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      fps_pos[0] += fps_front[0] * speed;
      fps_pos[1] += fps_front[1] * speed;
      fps_pos[2] += fps_front[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      fps_pos[0] -= fps_front[0] * speed;
      fps_pos[1] -= fps_front[1] * speed;
      fps_pos[2] -= fps_front[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      fps_pos[0] -= right[0] * speed;
      fps_pos[1] -= right[1] * speed;
      fps_pos[2] -= right[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      fps_pos[0] += right[0] * speed;
      fps_pos[1] += right[1] * speed;
      fps_pos[2] += right[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      fps_pos[1] -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      fps_pos[1] += speed;
    }
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y) {
  static float sensitivity = 0.1f;

  if (firstMouse) {
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    firstMouse = 0;
  }

  float dx = mouse_x - last_mouse_x;
  float dy = last_mouse_y - mouse_y; // reversed Y: up is positive

  last_mouse_x = mouse_x;
  last_mouse_y = mouse_y;

  yaw   += dx * sensitivity;
  pitch += dy * sensitivity;

  // Clamp pitch to avoid flipping
  if (pitch > 89.0f)  pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  // Recalculate FPS front vector
  float yaw_rad = yaw * (M_PI / 180.0f);
  float pitch_rad = pitch * (M_PI / 180.0f);

  fps_front[0] = cosf(pitch_rad) * cosf(yaw_rad);
  fps_front[1] = sinf(pitch_rad);
  fps_front[2] = cosf(pitch_rad) * sinf(yaw_rad);

  vec3_normalize(fps_front, fps_front);
}

// Add this to your mouse_button_callback to replace the existing logic
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    puts("click");

    vec3 ray_origin, ray_direction;
    vec3_copy(fps_pos, ray_origin);
    vec3_copy(fps_front, ray_direction);

    if (fabs(ray_direction[1]) > 0.0001f) {
      float t = -ray_origin[1] / ray_direction[1];
      if (t > 0.0f) {
        vec3 hit_point;
        vec3_scale(ray_direction, t, hit_point);
        vec3_add(ray_origin, hit_point, hit_point);
	vec3_copy(hit_point, ((raycast*)ray_thing.data)->point);
        vec3_copy(hit_point, rc.ripples[0].origin);
        rc.ripples[0].start_time = glfwGetTime();

        vec3_print(rc.ripples[0].origin);
	printf("%f\n", rc.ripples[0].start_time);
      }
    }
    
    /*
    float x_ndc = (2.0f * last_mouse_x) / screen_width - 1.0f;
    float y_ndc = 1.0f - (2.0f * last_mouse_y) / screen_height;

    vec4 ray_eye = { x_ndc, y_ndc, -1.0f, 0.0f }; // directional vector
    vec4_print(ray_eye);

    // convert to world space
    vec4 ray_world;
    mat4_mul_vec4(inv_view, ray_eye, ray_world);

    vec4 ray_dir;
    vec4_normalize(ray_world, ray_dir);

    vec3 ray_direction = { ray_dir[0], ray_dir[1], ray_dir[2] };
    vec3_normalize(ray_direction, ray_direction);

    vec3_print(ray_direction);
    */
  }
}
