#version 330 core


out vec4 FragColor;

in vec3 Pos;
in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform float time;

void main()
{
  float wrapped = sin(time*0.2);
  FragColor = texture(ourTexture, TexCoord)*vec4(wrapped, -wrapped, wrapped*wrapped, 1.0f); //texture(ourTexture, TexCoord); //vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 
