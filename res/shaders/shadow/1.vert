#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 offset;


out vec3 colors;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightMatrix;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
}vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));
    colors = color;
    vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
    vs_out.TexCoord = vTex;
    vs_out.FragPosLightSpace = lightMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}