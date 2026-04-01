#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos; //转换到观察空间

void main()
{
    FragPos = vec3(view * model * vec4(vPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * vNormal;
    TexCoords = vTex;
    gl_Position = projection * vec4(FragPos, 1.0);
}