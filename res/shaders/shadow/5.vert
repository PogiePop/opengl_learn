#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * vNormal;
    vs_out.TexCoord = vTex;
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}