
#version 330 core

layout (location = 0) in vec3 v_pos;

uniform mat4 model;
uniform mat4 proj;

uniform vec3 cam_pos;
uniform vec3 cam_front;
uniform vec3 cam_up;

uniform vec3 pos;

mat4 get_view(vec3 cam_front, vec3 cam_up, vec3 cam_pos);



mat4 scale = mat4(0.1, 0, 0, 0,
                  0, 0.1, 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);

void main() {
  mat4 view = get_view(cam_front, cam_up, cam_pos);
  
  
  gl_PointSize = 4.0;
  gl_Position  = proj * view * model * vec4(v_pos, 1.0f);
  
}


mat4 get_view(vec3 cam_front, vec3 cam_up, vec3 cam_pos) {
  vec3 d = normalize(-cam_front);       // dir
  vec3 r = normalize(cross(cam_up, d)); // right
  vec3 u = cross(d, r);                 // up
  
  
  return mat4(r.x, u.x, d.x, 0.0f,
              r.y, u.y, d.y, 0.0f,
              r.z, u.z, d.z, 0.0f,
              0,   0,   0,  1)
    * mat4(1,   0,   0,   0,
           0,   1,   0,   0,
           0,   0,   1,   0,
           -cam_pos.x, -cam_pos.y, -cam_pos.z, 1);
}
