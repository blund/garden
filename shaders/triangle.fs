#version 330 core

in vec3 frag_pos;
out vec4 frag_color;

vec3 base_color = vec3(0.0, 0.2, 0.4);   // deep water
vec3 peak_color = vec3(0.2, 0.8, 1.0);   // shallow water

void main()
{
  float height = smoothstep(0.0, 0.25, frag_pos.y);

  vec3 color = mix(base_color, peak_color, height);
  
  frag_color = vec4(color, 1.0); //vec4(1.0f, frag_pos.y*10.0f, 1.0f, 1.0f);
  //FragColor = vec4(frag_pos * 0.5 + 0.5, 1.0);
}
