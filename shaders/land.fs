#version 330 core

in vec3 frag_pos;
out vec4 frag_color;

vec3 base_color = vec3(0.1, 0.1, 0.1); // grey

void main()
{
  frag_color = vec4(base_color, 1.0);
}