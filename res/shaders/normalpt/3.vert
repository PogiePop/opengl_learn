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
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    vs_out.FragPos = vec3(model * vec4(vPos, 1.0));
    vs_out.TexCoord = vTex;
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * vNormal);
    mat3 tbn_inverser = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = tbn_inverser * lightPos;
    vs_out.TangentViewPos = tbn_inverser * viewPos;
    vs_out.TangentFragPos = tbn_inverser * vs_out.FragPos;
}