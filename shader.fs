#version 330 core
out vec4 fragment_farge;

in vec3 punkt_farge;
in vec2 trekk_koordinater;

uniform sampler2D trekk;

void main()
{

   fragment_farge = texture(trekk, trekk_koordinater) + vec4(punkt_farge, 0.0);
}