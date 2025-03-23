#version 300 es
layout (location = 0) in vec3 aPos;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_time;


void main() {
  gl_PointSize = 1.0;

  vec3 pos = aPos;
  
  //pos.y += 0.03 * sin(5.0 * pos.x + u_time) * cos(5.0 * pos.z + u_time*0.7);
  //pos.y += 0.03 * sin(5.0 * pos.x + u_time*1.6) * cos(5.0 * pos.z + u_time*1.2);
  pos.y += 0.03 * sin(1.0 * pos.x + u_time/2.0) * cos(1.0 * pos.z + u_time/2.0);
  pos.y += 0.02 * sin(3.0 * pos.x + u_time*1.0) * cos(3.0 * pos.z + u_time*1.0);
  pos.y += 0.02 * sin(8.0 * pos.x + u_time*1.5) * cos(8.0 * pos.z + u_time*1.5);
  /*
  if (u_ripple_start_time >= 0.0) {
    float ripple_time = u_time - u_ripple_start_time;
    float dist = distance(pos.xz, u_ripple_origin.xz);

    float wave = sin(20.0 * dist - ripple_time * 10.0);
    wave *= exp(-10.0 * dist);       // fade with distance
    wave *= exp(-2.0 * ripple_time); // fade with time

    pos.y += 0.5 * wave;
  }
  */

  gl_Position = projection * view * model * vec4(pos, 1.0);
  frag_pos = pos.xyz;
}
