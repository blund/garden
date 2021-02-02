// https://www.glfw.org/docs/latest/compile.html#compile_manual


#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>

#include "linalg.h"
#include "helpers.h"


void process_keyboard(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

typedef struct globals {
  float dt;
  int   first_mouse_reading;
} globals;

typedef struct camera {
  vec3 pos;
  vec3 front;
  vec3 up;
  
  float speed;
  float fov;
} camera;


static camera cam = {
  .speed  = 0.1f,
  .up     = {0.0f, 1.0f, 0.0f},
  .front  = {1.0f, 0.0f, 0.0f},
  .pos    = {0.0f, 0.0f, 3.0f},
  .fov    = 3.1415f
};


static globals global = {
  .first_mouse_reading = 1,
};



int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          "GARDEN", NULL, NULL);
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);  

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to start GLAD");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);


    //int lighting_shader = mkShader("shaders/color.fs", "shaders/color.vs");
    int fragment_shader = loadShader("shaders/color.fs", GL_FRAGMENT_SHADER);
    int vertex_shader   = loadShader("shaders/color.vs",   GL_VERTEX_SHADER);


    // Link shadere til et shader objekt, og slett shaderene
    int lighting_shader = glCreateProgram();
    glAttachShader(lighting_shader, vertex_shader);
    glAttachShader(lighting_shader, fragment_shader);
    glLinkProgram(lighting_shader);
    
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(lighting_shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(lighting_shader, 512, NULL, infoLog);
      printf("Failed to link fragment shader");
      printf("%s", infoLog);
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    
    float cube_model[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };

    float triangle[] = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      0.0f,  0.5f, 0.0f
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 
    

    //unsigned int texture = loadImage("data/wall.jpg");
    /*
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_model), cube_model, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    */
    // texture coord attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    /* unsigned int lightVAO; */
    /* glGenVertexArrays(1, &lightVAO); */
    /* glBindVertexArray(lightVAO); */
    /* glBindBuffer(GL_ARRAY_BUFFER, VBO); */
    /* glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void *)0); */
    /* glEnableVertexAttribArray(0); */

  


    mat4 proj = {0};   // Used for storing the projection matrix
    mat4 model = {0};  // Used for storing the model transform to be applied
    mat4 tmp; // Temporary matrix, used for operations to avoid allocations all the time :)

    float aspect_ratio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

    float last_time    = 0.0f;
    float current_time = 0.0f;
  
    //glfwSwapInterval(0.0f);
    while (!glfwWindowShouldClose(window)) {
        current_time = glfwGetTime();
        global.dt    = current_time - last_time;
        last_time    = current_time;

        process_keyboard(window);


        // Start rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glUseProgram(lighting_shader);

        clearM4(model);
        clearM4(proj);
        
        model[0] = 1.0f;
        model[4] = 1.0f;
        model[8] = 1.0f;

        
        
        printM4(model);
        
        setMat4(lighting_shader, "model", model);

        perspective(1.5f, aspect_ratio, 0.1f, 100.0f, proj);
        setMat4(lighting_shader, "proj",      proj);

        //printM4(proj);
        
        setVec3(lighting_shader, "cam_front", cam.front);
        setVec3(lighting_shader, "cam_pos",   cam.pos);
        setVec3(lighting_shader, "cam_up",    cam.up);

        //vec3 object_color = {1.0f, 0.5f, 0.31f};
        //        setVec3(lighting_shader, "object_color", object_color);

        // vec3 light_color  = {1.0f, 1.0f, 1.0f};
        //setVec3(lighting_shader, "light_color",  light_color);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}






void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    static const float sens = 0.1f;
    
    static float last_x;
    static float last_y;

    float yaw;
    float pitch;

    if (global.first_mouse_reading) {
        last_x = x_pos;
        last_y = y_pos;
        global.first_mouse_reading = 0;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    x_offset *= sens;
    y_offset *= sens;

    yaw   += x_offset;
    pitch += y_offset;

    if (pitch > 89.0f)  pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    pitch *= 0.01745329251f; // pi/180
    yaw   *= 0.01745329251f;

    cam.front[0] = cos(yaw) * cos(pitch);
    cam.front[1] = sin(pitch);
    cam.front[2] = sin(yaw) * cos(pitch);

    normalizeV3(cam.front);
}


void process_keyboard(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        cam.fov -= 0.01f;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        cam.fov += 0.01f;

    if (cam.fov < 0.01f)
        cam.fov = 0.01f;

    float speed = 7.5f * global.dt;
   
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 tmp;
        copyV3(cam.front, tmp);
        scaleV3(tmp, speed);
        addV3(cam.pos, tmp);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 tmp;
        copyV3(cam.front, tmp);
        scaleV3(tmp, speed);
        subV3(cam.pos, tmp);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 tmp;
        crossV3(cam.front, cam.up, tmp);
        normalizeV3(tmp);
        scaleV3(tmp, speed);
        subV3(cam.pos, tmp);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 tmp;
        crossV3(cam.front, cam.up, tmp);
        normalizeV3(tmp);
        scaleV3(tmp, speed);
        addV3(cam.pos, tmp);
    }
}

