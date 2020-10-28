
#define STB_DEFINE

#include <GLFW/glfw3.h>

#include <ao/ao.h>
#include <stb/stb.h>

static void error_callback(int error, const char *description) {
  fputs(description, stderr);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

#define SAMPLE_FREQUENCY 44100
#define FRAME_RATE 60

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
  GLFWwindow *window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  window = glfwCreateWindow(640, 480, "Ducks at the Lake", NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef((float)glfwGetTime() * 50.f, 2.f, 0.f, 1.f);

    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.6f, -0.4f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.6f, -0.4f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.6f, 0.f);

    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();

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

    ao_play(device, buffer, buf_size);
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
