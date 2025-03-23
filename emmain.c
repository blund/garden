#include <emscripten/emscripten.h>
#include <GLES3/gl3.h>

#include <stdio.h>



#include <GLFW/glfw3.h>

#define BL_IMPL
#include "bl.h"

#define READFILE_IMPL
#include "read_file.h"

#define LINALG_IMPL
#include "linalg.h"

#include "thing.h"
#include "things/waves.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);

vec3 fps_pos = {0.0f, 0.8f, 10.0f}; // 1 unit above ground
vec3 fps_front = {0.0f, 0.0f, -1.0f}; // looking forward on Z
vec3 fps_up = {0.0f, 1.0f, .0f};     // global up direction

float camera_angle = 0;

float lastX = 400, lastY = 300;
float yaw = -90.0f; // left/right
float pitch = 0.0f; // up/down

int firstMouse = 1;

float cameraDistance = 2.0f;

thing w;
GLFWwindow* window;


void main_loop();

int main() {
  // load glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // set up window
  GLFWwindow* window = glfwCreateWindow(800, 600, "Welcome to the Garden", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "failed to create glfw window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // set initial viewport
  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  // set framebuffer resize callback :)
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  compile_shader(&w, "shaders/glsl/triangle.vs", "shaders/glsl/triangle.fs");
  create_waves(&w);

  // main loop
  emscripten_set_main_loop(main_loop, 0, 1);

  // terminate program
  glfwTerminate();
  printf("Nothing's really happened like I thought it would\n");
  return 0;
}

void main_loop() {
  process_input(window);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
  mat4 projection;
  float aspect = 800.0f / 600.0f; // use your real window size
  mat4_perspective(45.0f, aspect, 0.1f, 100.0f, projection);

  mat4 view;
  vec3 center;
  vec3_add(fps_pos, fps_front, center);
  mat4_lookat(fps_pos, center, fps_up, view);

  mat4 model;
  mat4_identity(model);
  mat4_scale(model, 10.0f, 4.0f, 10.0f);
    
  float time = (float)glfwGetTime();

  glUseProgram(w.shader_program);
  set_uniforms(&w, projection, view, model, time);
  render_waves(&w);
 
  // finish render!
  glfwSwapBuffers(window);
  glfwPollEvents();    
}

// on resize, simply set new bounds
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
      puts("lshift!");
      fps_pos[1] -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      puts("space!");
      fps_pos[1] += speed;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  static float sensitivity = 0.1f;

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = 0;
  }

  float dx = xpos - lastX;
  float dy = lastY - ypos; // reversed Y: up is positive

  lastX = xpos;
  lastY = ypos;

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
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  printf("%f\n", cameraDistance);
  cameraDistance -= yoffset * 0.1f;
  if (cameraDistance < 0.5f) cameraDistance = 0.5f;
  if (cameraDistance > 10.0f) cameraDistance = 10.0f;
}
