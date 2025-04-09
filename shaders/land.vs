#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vec3 pos = aPos;

  gl_Position = projection * view * model * vec4(pos, 1.0);
  frag_pos = pos.xyz;
}
