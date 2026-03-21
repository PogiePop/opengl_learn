#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

in VS_OUT
{
    mat3 TBN;
    vec2 texCoord;
    vec3 viewPos;
}gs_in[];

uniform mat4 projection;
uniform float magnitude = 0.2f;
struct Material{
    sampler2D normal0;
};

uniform Material material;
out vec3 color;
const vec3 RED = vec3(1.0, 0.0, 0.0);
const vec3 GREEN = vec3(0.0, 1.0, 0.0);
const vec3 BLUE = vec3(0.0, 0.0, 1.0);

void GenerateLine(int index)
{
    // 检查TBN矩阵是否正交
    vec3 T = gs_in[index].TBN[0];
    vec3 B = gs_in[index].TBN[1];
    vec3 N = gs_in[index].TBN[2];

    // 显示切线、副切线、法线
    // 切线 - 红色
    color = RED;
    gl_Position = projection * vec4(gs_in[index].viewPos, 1.0);
    EmitVertex();
    gl_Position = projection * (vec4(gs_in[index].viewPos, 1.0) + vec4(T, 0.0) * magnitude);
    EmitVertex();
    EndPrimitive();

    // 副切线 - 绿色
    color = GREEN;
    gl_Position = projection * vec4(gs_in[index].viewPos, 1.0);
    EmitVertex();
    gl_Position = projection * (vec4(gs_in[index].viewPos, 1.0) + vec4(B, 0.0) * magnitude);
    EmitVertex();
    EndPrimitive();

    // 法线 - 蓝色
    color = BLUE;
    gl_Position = projection * vec4(gs_in[index].viewPos, 1.0);
    EmitVertex();
    gl_Position = projection * (vec4(gs_in[index].viewPos, 1.0) + vec4(N, 0.0) * magnitude);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}