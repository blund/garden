// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php
// https://www.geeksforgeeks.org/multithreading-c-2/

// http://www.science.smith.edu/dftwiki/index.php/Tutorial:_Playing_Sounds_in_a_Separate_Thread

// https://computing.llnl.gov/tutorials/pthreads/#PassingArguments

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <time.h>
#include <math.h>
#include <pthread.h>

// #include <ao/ao.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD //@Ytelse: Siden vi bruker tcc nå kan vi ikke gjøre simd-operasjoner. Dette må fjernes når vi skal kompilere for at ting faktisk skal kjøre kjapt, som med gcc eller lignende.

#include <stb/stb_image.h>

#define SAMPLE_FREQUENCY 44100
#define FRAME_RATE 60

char *loadFile(char *file_name);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


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
  /*
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

  // -- Setup for default driver --
  ao_initialize();
  default_driver = ao_default_driver_id();
  default_driver = ao_default_driver_id();

  memset(&format, 0, sizeof(format));
  format.bits = 16;
  format.channels = 2;
  format.rate = 44100;
  format.byte_format = AO_FMT_LITTLE;


   -- Open driver -- 
  ao_option *ao_options = NULL;

  device = ao_open_live(default_driver, &format, ao_options);
  if (device == NULL) {
    fprintf(stderr, "Error opening device.\n");
    return 1;
  }
  */
  /*
  //  Play some stuff
  //  buf_size = format.bits / 8 * format.channels * format.rate/8;
  buf_size = format.bits / 8 * format.channels * buffer_samples;
  buffer = (char *)calloc(buf_size, sizeof(char));
  */
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
  // Last inn og kompiler vertex-shader-program
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  char vertex_shader_file_name[] = "shader.vs";
  char *vertex_shader_source = loadFile(vertex_shader_file_name);
  glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("Failed to compile vertex shader");
    printf("%s", infoLog);
    return 1;
  }



  // Last inn og kompiler fragment-shader-program
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  char fragment_shader_file_name[] = "shader.fs";
  char *fragment_shader_source = loadFile(fragment_shader_file_name);
  glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("Failed to compile fragment shader");
    printf("%s", infoLog);
    return 1;
  }



  // Link shadere til et shader objekt, og slett shaderene
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Failed to link fragment shader");
    printf("%s", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);





  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float punkt_verdier[] = {
    // venstre nede
    -0.5f, -0.5f, 0.0f, // koordinat
    1.0f, 0.0f, 0.0f,   // farge
    0.0f, 0.0f,         // tekstur-koordinat

    // høyre nede
    0.5f,  -0.5f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f,  

    // venstre oppe
    -0.5f,  0.5f,  0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f,

     // høyre oppe
    0.5f,  0.5f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f,  

 };
  unsigned int indekser[] = {
    0, 1, 2, // første trekant
    1, 2, 3,
  };

  float trekk_koordinater[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };


  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  int width, height, nrChannels;
  unsigned char *data = stbi_load("ressurser/wall.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Klarte ikke å laste inn bildet!");
  }


  stbi_image_free(data);

  
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(punkt_verdier), punkt_verdier, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indekser), indekser, GL_STATIC_DRAW);
  
  // Her forklarer vi verdiene til delene av punkt_verdier. Dette innebærer posisjonen til denne attributten, hvor mange elementer den har, hvilke typer den består av, hvor mange som er i totalt og hvor stor offset fra begynnelsen denne attributten har.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6*sizeof(float)));
  glEnableVertexAttribArray(2);

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
    struct timeval render_start, render_stop;
    double draw_secs = 0;
    double render_secs = 0;


    // input
    // -----
    processInput(window);

    // render
    // ------
    //gettimeofday(&render_start, NULL);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    float tid = glfwGetTime();
    float grønn_verdi = (sin(tid) / 2.0f) + 0.5f;
    float rød_verdi = (sin(tid*2) / 2.0f) + 0.5f;
    float blå_verdi = (sin(tid*3) / 2.0f) + 0.5f;

    // int trekk_uniform = glGetUniformLocation(shaderProgram, "trekk");
    //glUseProgram(shaderProgram);
    //glUniform2f(vertex_farge_sted, rød_verdi, grønn_verdi, blå_verdi, 1.0f);
    
    // draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(
        VAO); // seeing as we only have a single VAO there's no need to bind it
              // every time, but we'll do so to keep things a bit more organized
    //gettimeofday(&render_stop, NULL);

    //gettimeofday(&draw_start, NULL);

    gettimeofday(&render_stop, NULL);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //gettimeofday(&draw_stop, NULL);
    // glBindVertexArray(0); // no need to unbind it every time

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------




    /*
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
    */


    //ao_play(device, buffer, buf_size);


    draw_secs = (double)(draw_stop.tv_usec - draw_start.tv_usec) / 1000000 + (double)(draw_stop.tv_sec - draw_start.tv_sec);

    render_secs = (double)(render_stop.tv_usec - render_start.tv_usec) / 1000000 + (double)(render_stop.tv_sec - render_start.tv_sec);
    //printf("render %f | draw %f\n",render_secs*1000, draw_secs*1000);


    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  // shutdown audio
  //ao_close(device);
  //ao_shutdown();

  // shutdown video
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);

  return 0;
}
