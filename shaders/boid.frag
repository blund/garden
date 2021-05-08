#version 330 core
out vec4 color;


in vec3 Frag_pos;

void main()
{
   // color = texture(texture0, texture_coord);
  
   color = 0.6*vec4(1.0, 1.0, 1.0, 1.0);
}
