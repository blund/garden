#version 300 es
precision mediump float;

in vec3 frag_pos;
out vec4 frag_color;

void main()
{
    vec3 base_color = vec3(0.0, 0.2, 0.4);   // deep water
    vec3 peak_color = vec3(0.2, 0.8, 1.0);   // shallow water

    float height = smoothstep(0.0, 0.25, frag_pos.y);
    vec3 color = mix(base_color, peak_color, height);

    frag_color = vec4(color, 1.0);
}