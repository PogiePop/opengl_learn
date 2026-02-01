#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 offsets[100];

void main()
{
    vec3 offset = offsets[gl_InstanceID];
    gl_Position = projection * view * model * vec4(vPos + offset, 1.0);
    TexCoord = vTex;
}