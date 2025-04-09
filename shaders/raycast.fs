#version 330 core

in vec3 frag_pos;
out vec4 frag_color;

vec3 point_color = vec3(1.0, 0.0, 0.2);   // deep water

void main()
{
  frag_color = vec4(point_color, 1.0);
}
