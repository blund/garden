#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 frag_pos;
out float ripple_amp;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_time;

uniform vec3 u_ripple_origin;
uniform float u_ripple_start_time;


float base_wave(vec3 pos, float u_time) {
  float wave1 = 0.05 * sin(1.0 * pos.x + u_time * 0.7) * cos(1.0 * pos.z + u_time * 0.4);
  float wave2 = 0.03 * sin(3.0 * pos.x - u_time * 1.2) * cos(2.0 * pos.z + u_time * 0.6);
  float wave3 = 0.01 * sin(4.0 * pos.z + u_time * 0.9) * cos(2.0 * pos.x - u_time * 0.3);
  float wave4 = 0.005 * sin(10.0 * pos.x + u_time * 1.5); // small fine ripples

  return wave1 + wave2 + wave3 + wave4;
}

float add_ripples(vec3 pos, float u_time, vec3 origin, float start_time) {
  if (start_time < 0.0) return 0.0;

  float t = u_time - start_time;
  float dist = distance(pos.xz, origin.xz);

  float frequency = 22.0;
  float amplitude = 0.01;
  float speed = 2.0;

  float wavelength = t * speed;
  float slowed_t = log(1.0 + t);

  // Oscillating wave
  float wave = sin(frequency * dist - frequency * slowed_t);

  // Envelope around wavefront
  float envelope = exp(-pow(dist - wavelength, 2.0) * 20.0);

  // Fade-in (suppress early waves)
  float fade_in = smoothstep(0.05, 0.4, t); // tweak range

  // Fade-out over time
  float fade_out = exp(-2.0 * t);

  return amplitude * wave * envelope * fade_in * fade_out;
}

void main() {
  gl_PointSize = 1.0;

  vec3 pos = aPos;

  pos.y += base_wave(pos, u_time);
  float ripples = add_ripples(pos, u_time, u_ripple_origin,  u_ripple_start_time);
  ripples = clamp(-10000, 0.06, 30*ripples);
  pos.y += ripples;
  gl_Position = projection * view * model * vec4(pos, 1.0);
  ripple_amp = 4000*ripples;
  frag_pos = pos.xyz;
}
