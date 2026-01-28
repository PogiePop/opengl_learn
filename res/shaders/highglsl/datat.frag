#version 330 core

layout (std140) uniform Matrix
{
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 color;
};

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
}