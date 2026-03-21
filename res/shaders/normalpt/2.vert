#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTex;
layout (location = 2) in vec3 vNormal;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT{
    mat3 TBN;
    vec2 texCoord;
    vec3 viewPos;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    mat4 translate = transpose(inverse(model));
    vec3 viewT = normalize(mat3(translate) * tangent);
    //vec3 viewB = normalize(mat3(translate) * bitangent);
    vec3 viewN = normalize(mat3(translate) * vNormal);
    viewT = normalize(viewT - dot(viewT, viewN) * viewN);
    vec3 viewB = cross(viewN, viewT);
    mat3 v = mat3(view);
    vs_out.TBN = mat3(v * viewT, v * viewB, v * viewN);
    vs_out.texCoord = vTex;
    vs_out.viewPos = vec3(view * model * vec4(vPos, 1.0));
}