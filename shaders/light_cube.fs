#version 330 core
out vec4 color;

in vec2 texture_coord;

// uniform sampler2D texture0;
uniform vec3      light_color;

void main()
{
   // color = texture(texture0, texture_coord);
   color = vec4(light_color, 1.0);
}
