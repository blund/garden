#version 330 core

layout (location = 0) in vec3 v_posisjon;
layout (location = 1) in vec3 v_punkt_farge;
layout (location = 2) in vec2 v_trekk_koordinater;

out vec3 punkt_farge;
out vec2 trekk_koordinater;

void main()
{

   gl_Position = vec4(v_posisjon, 1.0);
   punkt_farge = v_punkt_farge;
   trekk_koordinater = v_trekk_koordinater;

}