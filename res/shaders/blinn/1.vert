#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 offset;

out vec2 TexCoord;
out vec3 colors;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;


void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    TexCoord = vTex;
    colors = color;
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = vec3(transpose(inverse(model)) * vec4(vNormal, 1.0));
}