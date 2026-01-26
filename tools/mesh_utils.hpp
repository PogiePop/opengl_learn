#ifndef _WA_MESH_UTILS_HPP
#define _WA_MESH_UTILS_HPP
#include <mesh.hpp>

const static std::vector<Texture> _empty = std::vector<Texture>();

Mesh CreateCubeMesh(float, const std::vector<Texture> &);
Mesh CreateQuadMesh(float, const std::vector<Texture> &);

Mesh CreateCubeMesh(float size = 1.0f, const std::vector<Texture> &textures = _empty)
{
    // 计算半长（立方体中心在原点，各轴范围 [-halfSize, halfSize]）
    float halfSize = size / 2.0f;

    // 立方体顶点数据（24个顶点：6个面 × 4个顶点）
    // 核心修正：所有面的顶点顺序统一为「从外部看逆时针」
    std::vector<Vertex> cubeVertices = {
        // 后平面 (z = -halfSize) - 法线：(0,0,-1)
        // 顶点顺序：左下 → 右下 → 右上 → 左上（外部看逆时针）
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{halfSize, halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-halfSize, halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

        // 前平面 (z = halfSize) - 法线：(0,0,1)
        // 顶点顺序：右下 → 左下 → 左上 → 右上（外部看逆时针）
        {{halfSize, -halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-halfSize, -halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-halfSize, halfSize, halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{halfSize, halfSize, halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

        // 左平面 (x = -halfSize) - 法线：(-1,0,0)
        // 顶点顺序：右上 → 右下 → 左下 → 左上（外部看逆时针）
        {{-halfSize, halfSize, halfSize}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize, -halfSize, halfSize}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize, halfSize, -halfSize}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

        // 右平面 (x = halfSize) - 法线：(1,0,0)
        // 顶点顺序：左上 → 左下 → 右下 → 右上（外部看逆时针）
        {{halfSize, halfSize, -halfSize}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{halfSize, -halfSize, halfSize}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{halfSize, halfSize, halfSize}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

        // 下平面 (y = -halfSize) - 法线：(0,-1,0)
        // 顶点顺序：左上 → 右上 → 右下 → 左下（外部看逆时针）
        {{-halfSize, -halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{halfSize, -halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

        // 上平面 (y = halfSize) - 法线：(0,1,0)
        // 顶点顺序：左下 → 右下 → 右上 → 左上（外部看逆时针）
        {{-halfSize, halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{halfSize, halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{halfSize, halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-halfSize, halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}};

    // 原立方体索引（顺时针），替换为以下逆时针索引即可
    std::vector<unsigned int> cubeIndices = {
        0, 3, 2, 0, 2, 1,       // 后平面 反向
        4, 7, 6, 4, 6, 5,       // 前平面 反向
        8, 11, 10, 8, 10, 9,    // 左平面 反向
        12, 15, 14, 12, 14, 13, // 右平面 反向
        16, 19, 18, 16, 18, 17, // 下平面 反向
        20, 23, 22, 20, 22, 21  // 上平面 反向
    };

    // 创建并返回Mesh对象
    return Mesh(cubeVertices, cubeIndices, textures);
}

Mesh CreateQuadMesh(float size = 1.0f, const std::vector<Texture> &textures = _empty)
{
    // 半长：四边形沿X/Y轴的半尺寸，中心在(0,0,0)，平面Z=0
    float halfSize = size / 2.0f;

    // 4个顶点：左下、右下、右上、左上（符合右手坐标系，纹理坐标常规映射）
    // 顶点结构：position(glm::vec3) + texCoord(glm::vec2) + normal(glm::vec3)
    std::vector<Vertex> quadVertices = {
        // 左下 (X-, Y-, Z0) | 纹理(0,0) | 法线+Z
        {{-halfSize, -halfSize, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        // 右下 (X+, Y-, Z0) | 纹理(1,0) | 法线+Z
        {{halfSize, -halfSize, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        // 右上 (X+, Y+, Z0) | 纹理(1,1) | 法线+Z
        {{halfSize, halfSize, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // 左上 (X-, Y+, Z0) | 纹理(0,1) | 法线+Z
        {{-halfSize, halfSize, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}};

    // 6个索引：2个三角形拼接四边形（0-1-2 + 0-2-3），无顶点复用
    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        0, 2, 3};

    // 直接返回Mesh对象，复用现有Mesh类的VAO/VBO/EBO初始化逻辑
    return Mesh(quadVertices, quadIndices, textures);
}

#endif