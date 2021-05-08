#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform float width;
uniform float height;

out vec2 TexCoord;
out vec3 Pos;

void main() {
  float ratio = height/width;
  float scale = .5;
  //float pos_x = -1.23;
  //float pos_y = -0.73;

  TexCoord = aTexCoord;

  
  gl_Position =  vec4(aPos.x*ratio - distance(aPos/scale, vec3(0)),
                      aPos.y - distance(aPos/scale, vec3(0)),
                      aPos.z,
                      1.0 / scale);

  Pos = aPos;
 
}
