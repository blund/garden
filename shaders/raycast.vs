#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_time;

void main() {
  gl_PointSize = 100.0;

  gl_Position = projection * view * model * vec4(aPos, 1.0);
  frag_pos = aPos.xyz;
}
