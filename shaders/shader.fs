#version 330 core

struct Light {
  vec3 direction;
  
  vec3  ambient;
  vec3  diffuse;
  vec3  specular;
  float shininess;
};

struct Material {
  vec3  ambient;
  vec3  diffuse;
  vec3  specular;
  float shininess;
};


out vec4 color;


//in vec2 texture_coord;
in vec3 Normal;
in vec3 Frag_pos;

//uniform sampler2D texture0;

uniform Material material;
uniform Light    light;

uniform vec3 cam_front;
uniform vec3 cam_pos;

uniform vec3 view_pos;
uniform vec3 object_color;

uniform vec3 light_color;
uniform vec3 light_pos;


void main()
{
  // Brukes for å la lysstyrken avta ved avstand!
  // Større 'distance_factor' gir lysere lys.
  float distance_factor = 1.0;
  float recip_distance  = min(1.0, 1/(0.1*distance(light_pos, Frag_pos)));


  
  // Beregn ambient lys
  vec3  ambient =  light.ambient * material.ambient;
 
  // Beregn diffust lys
  vec3 norm      = normalize(Normal);
  //vec3 light_dir = normalize(light_pos - Frag_pos);
  vec3 light_dir = normalize(-light.direction);

  float diff     = max(dot(norm, light_dir), 0.0);
  vec3  diffuse  = light.specular * material.specular * diff;

  //
  // Beregn spekulært lys
  vec3  view_dir    = normalize(cam_pos - Frag_pos);
  vec3  reflect_dir = reflect(-light_dir, norm);
  float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
  vec3  specular    = light.specular * material.specular * spec; 

  
  //
  // Sett sammen lysene!!
  vec3 light = (ambient + diffuse + specular) * object_color; // * recip_distance;

 

  float distance = sqrt(pow(gl_FragCoord.x-450, 2) + pow(gl_FragCoord.y - 300, 2));
  color = vec4(light, 0.3);// - distance * 0.001;// + 100/ (gl_FragCoord.x + gl_FragCoord.y/2);
      
  
  
  //color = texture(texture0, texture_coord);
}
