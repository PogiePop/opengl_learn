#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;


out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Normal;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));
    vs_out.TexCoord = vTex;
    vs_out.Normal = vec3(transpose(inverse(model)) * vec4(vNormal, 1.0));
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}