#version 330 core
// 输入顶点属性（和你Mesh类中Vertex结构体一一对应）
layout (location = 0) in vec3 aPos;      // 顶点位置（layout 0）
layout (location = 1) in vec2 aTexCoord; // 纹理坐标（layout 1）
layout (location = 2) in vec3 aNormal;   // 法向量

// 输出到片段着色器的纹理坐标
out vec2 TexCoord;

// MVP矩阵
uniform mat4 model;       // 模型矩阵
uniform mat4 view;        // 视图矩阵（摄像机）
uniform mat4 projection;  // 投影矩阵

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}