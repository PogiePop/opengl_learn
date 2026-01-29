#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// out vec3 Normal;
// out vec3 FragPos;

out VS_OUT
{
    vec2 TexCoords;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    vs_out.TexCoords = vTex;
    // Normal = mat3(transpose(inverse(model))) * vNormal;
    // FragPos = vec3(model * vec4(vPos, 1.0));
}