
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define READFILE_IMPL
#include "read_file.h"

#include "linalg.h"

#include "thing.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);

vec3 fps_pos = {0.0f, 0.5f, 2.0f}; // 1 unit above ground
vec3 fps_front = {0.0f, 0.0f, -1.0f}; // looking forward on Z
vec3 fps_up = {0.0f, 1.0f, 0.0f};     // global up direction

float camera_angle = 0;

float lastX = 400, lastY = 300;
float yaw = -90.0f; // left/right
float pitch = 0.0f; // up/down

int firstMouse = 1;

float cameraDistance = 2.0f;

int main() {
  // load glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // set up window
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "failed to create glfw window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // load GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "failed to initialize GLAD\n");
    return -1;
  }

  // set initial viewport
  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  // set framebuffer resize callback :)
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


  // construct grid of 64x64 points with XYZ coordinates in a linear array
  const int n_per_side = 64;
  const int array_size = 3 * n_per_side * n_per_side;
  printf("%d\n", array_size);
  float points[array_size];
  
  float start = -1;
  float end   = 1;

  float len  = end - start;
  float step = len / (n_per_side - 1);

  for (int x = 0; x < n_per_side; x++) {
    for (int y = 0; y < n_per_side; y++) {
      int i = 3 * (x + y*n_per_side);
      points[i + 0] = -1.0f + x * step; // x
      points[i + 1] = frand(0, 0.05);              // y
      points[i + 2] = -1.0f + y * step; // <

      //printf("%f, %f\n", points[x+y], points[x+y+1]);
    }
  }

  const int squares_per_side = n_per_side - 1;
  const int num_cells = squares_per_side * squares_per_side;
  const int index_count = num_cells * 6;

  GLuint indices[index_count];
  int idx = 0;

  for (int y = 0; y < squares_per_side; y++) {
    for (int x = 0; x < squares_per_side; x++) {
      int top_left     = y * n_per_side + x;
      int top_right    = top_left + 1;
      int bottom_left  = top_left + n_per_side;
      int bottom_right = bottom_left + 1;

      // Triangle 1
      indices[idx++] = top_left;
      indices[idx++] = bottom_left;
      indices[idx++] = bottom_right;

      // Triangle 2
      indices[idx++] = top_left;
      indices[idx++] = bottom_right;
      indices[idx++] = top_right;
    }
  }

  thing t;
  compile_shader(&t, "shaders/triangle.vs", "shaders/triangle.fs");

  // Set up VAO for thing
  glGenVertexArrays(1, &t.vao);
  glGenBuffers(1, &t.ebo);
  glGenBuffers(1, &t.vbo);

  glBindVertexArray(t.vao);

  glBindBuffer(GL_ARRAY_BUFFER, t.vbo);
  glBufferData(GL_ARRAY_BUFFER, array_size*sizeof(float), points, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  
  // main loop
  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(t.shader_program);

    mat4 model;
    mat4_identity(model);

    mat4 view;
    vec3 center;
    vec3_add(fps_pos, fps_front, center);
    mat4_lookat(fps_pos, center, fps_up, view);
    
    mat4 projection;
    float aspect = 800.0f / 600.0f; // use your real window size
    mat4_perspective(45.0f, aspect, 0.1f, 100.0f, projection);

    uint32_t shader_program = t.shader_program;
    
    int projection_location = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE,
                       (const float *)projection);
    
    int view_location = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float *)view);

    int model_location = glGetUniformLocation(shader_program, "model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, (const float *)model);

    float time = (float)glfwGetTime();
    int time_location = glGetUniformLocation(shader_program, "u_time");
    glUniform1f(time_location, time);


    // glDrawArrays(GL_POINTS, 0, array_size / 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(t.vao);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
  
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
      fps_pos[1] -= speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
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
