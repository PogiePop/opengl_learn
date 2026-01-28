#version 330 core
layout (location = 0) in vec3 vPos;


layout (std140) uniform Matrix
{
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 color;
};



void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}