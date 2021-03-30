#version 330 core

// https://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;

//out vec2 texture_coord;
out vec3 Normal;
out vec3 Frag_pos;

uniform mat4 model;
uniform mat4 proj;


uniform vec3 cam_pos;
uniform vec3 cam_front;
uniform vec3 cam_up;

mat4 view;

void main()
{
   
   vec3 d = normalize(-cam_front);        // dir
   vec3 r = normalize(cross(cam_up, d));  // right
   vec3 u = cross(d, r);                  // up
   
   // lookAt
   view = mat4(r.x, u.x, d.x, 0.0f,
               r.y, u.y, d.y, 0.0f,
               r.z, u.z, d.z, 0.0f,
               0,   0,   0,  1)
     * mat4(1,   0,   0,   0,
            0,   1,   0,   0,
            0,   0,   1,   0,
            -cam_pos.x, -cam_pos.y, -cam_pos.z, 1);
   


   Normal   = mat3(transpose(inverse(model))) * v_normal;
   Frag_pos = vec3(model * vec4(v_pos, 1.0));

   gl_Position = proj * view * model * vec4(v_pos, 1.0f);

   //texture_coord = vec2(v_texture_coord.x, 1.0f - v_texture_coord.y);
}








