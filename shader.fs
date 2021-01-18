#version 330 core

precision mediump float;

out vec4 color;

in vec2 texture_coord;

uniform sampler2D texture0;

void main()
{
   color = texture(texture0, texture_coord);
}
