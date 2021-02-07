#version 330 core
out vec4 color;

//in vec2 texture_coord;
in vec3 Normal;
in vec3 Frag_pos;

//uniform sampler2D texture0;

uniform vec3 cam_front;
uniform vec3 cam_pos;

uniform vec3 view_pos;
uniform vec3 object_color;

uniform vec3 light_color;
uniform vec3 light_pos;


void main()
{
  // Beregn ambient lys
  float ambient_strength = 0.1;
   
  vec3  ambient = ambient_strength * light_color;

  // Beregn diffust lys
  vec3 norm      = normalize(Normal);
  vec3 light_dir = normalize(light_pos - Frag_pos);
  
  float diff     = max(dot(norm, light_dir), 0.0);
  vec3  diffuse  = diff * light_color;

  // Beregn spekulært lys
  float specular_strength = 0.5;
  int   shininess         = 32;

  vec3 view_dir    = normalize(cam_pos - Frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec     = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
  vec3  specular = specular_strength * spec * light_color; 
  //specular = vec3(0);

  // Make it shine!!
  vec3 light = (ambient + diffuse + specular) * object_color;
  color = vec4(light, 1.0);

  //color = texture(texture0, texture_coord);
}
