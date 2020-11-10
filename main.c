// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php
// https://www.geeksforgeeks.org/multithreading-c-2/

// http://www.science.smith.edu/dftwiki/index.php/Tutorial:_Playing_Sounds_in_a_Separate_Thread

// https://computing.llnl.gov/tutorials/pthreads/#PassingArguments

#define STB_DEFINE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <time.h>
#include <pthread.h>

#include <ao/ao.h>

#include <stb/stb.h>

#define SAMPLE_FREQUENCY 44100
#define FRAME_RATE 60

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void *PlaySound(void *threadid) {

}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
int main(void) {
  ao_device *device;
  ao_sample_format format;
  int default_driver;

  char *buffer;
  int buf_size;

  int sample1;
  int sample2;
  int mix;

  int buffer_samples = SAMPLE_FREQUENCY / FRAME_RATE;
  int sample_counter = 0;

  float freq1 = 440.0;
  float freq2 = 733.0;

  /* -- Setup for default driver -- */
  ao_initialize();
  default_driver = ao_default_driver_id();
  default_driver = ao_default_driver_id();

  memset(&format, 0, sizeof(format));
  format.bits = 16;
  format.channels = 2;
  format.rate = 44100;
  format.byte_format = AO_FMT_LITTLE;

  /* -- Open driver -- */
  ao_option *ao_options = NULL;

  device = ao_open_live(default_driver, &format, ao_options);
  if (device == NULL) {
    fprintf(stderr, "Error opening device.\n");
    return 1;
  }

  //  Play some stuff
  //  buf_size = format.bits / 8 * format.channels * format.rate/8;
  buf_size = format.bits / 8 * format.channels * buffer_samples;
  buffer = (char *)calloc(buf_size, sizeof(char));

  /* -- GL setup -- */

  // ---
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to make windows");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to start GLAD");
    return -1;
  }

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("Failed to compile vertex shader");
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("Failed to compile fragment shader");
  }
  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Failed to link fragment shader");
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      0.5f,  -0.5f, 0.0f, // right
      0.0f,  0.5f,  0.0f  // top
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.
  glBindVertexArray(0);
  // ---

  glfwMakeContextCurrent(window);
  // glfwSetKeyCallback(window, key_callback);

  while (!glfwWindowShouldClose(window)) {
    struct timeval draw_start, draw_stop;
    struct timeval audio_start, audio_stop;
    double draw_secs = 0;
    double audio_secs = 0;


    // input
    // -----
    processInput(window);

    // render
    // ------
    gettimeofday(&draw_start, NULL);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(
        VAO); // seeing as we only have a single VAO there's no need to bind it
              // every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0); // no need to unbind it every time

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------

    gettimeofday(&draw_stop, NULL);



    gettimeofday(&audio_start, NULL);
    // Render audio
    for (int i = 0; i < buffer_samples; i++) {
      sample1 =
          (int)(0.01 * 32768.0 *
                sin(2 * M_PI * freq1 * ((float)sample_counter / format.rate)));

      sample2 =
          (int)(0.011 * 32768.0 *
                sin(2 * M_PI * freq2 * ((float)sample_counter / format.rate)));

      mix = sample1 + sample2;

      // Put the same stuff in left and right channel
      buffer[4 * i + 0] = buffer[4 * i + 2] = mix & 0xff;
      buffer[4 * i + 1] = buffer[4 * i + 3] = (mix >> 8) & 0xff;

      sample_counter++;
    }

    gettimeofday(&audio_stop, NULL);


    ao_play(device, buffer, buf_size);


    draw_secs = (double)(draw_stop.tv_usec - draw_start.tv_usec) / 1000000 + (double)(draw_stop.tv_sec - draw_start.tv_sec);

    audio_secs = (double)(audio_stop.tv_usec - audio_start.tv_usec) / 1000000 + (double)(audio_stop.tv_sec - audio_start.tv_sec);
    printf("draw %f | audio %f\n",draw_secs*1000, audio_secs*1000);


    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  // shutdown audio
  ao_close(device);
  ao_shutdown();

  // shutdown video
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);

  return 0;
}
