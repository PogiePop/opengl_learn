#ifndef _WA_MESH_UTILS_HPP
#define _WA_MESH_UTILS_HPP
#include <mesh.hpp>

const static std::vector<Texture> _empty = std::vector<Texture>();

Mesh CreateCubeMesh(float, const std::vector<Texture>&);


Mesh CreateCubeMesh(float size = 1.0f, const std::vector<Texture>&textures = _empty) {
    // 计算半长（立方体中心在原点，各轴范围 [-halfSize, halfSize]）
    float halfSize = size / 2.0f;

    // 立方体顶点数据（24个顶点：6个面 × 4个顶点）
    // 每个顶点包含：位置(position)、纹理坐标(texCoord)、法线(normal)
    std::vector<Vertex> cubeVertices = {
        // 后平面 (z = -halfSize) - 法线：(0,0,-1)
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

        // 前平面 (z = halfSize) - 法线：(0,0,1)
        {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ halfSize,  halfSize,  halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

        // 左平面 (x = -halfSize) - 法线：(-1,0,0)
        {{-halfSize,  halfSize,  halfSize}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

        // 右平面 (x = halfSize) - 法线：(1,0,0)
        {{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

        // 下平面 (y = -halfSize) - 法线：(0,-1,0)
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ halfSize, -halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

        // 上平面 (y = halfSize) - 法线：(0,1,0)
        {{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-halfSize,  halfSize,  halfSize}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}
    };

    // 立方体索引数据（36个索引：6个面 × 2个三角形 × 3个顶点）
    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 0, 2, 3,   // 后平面
        4, 5, 6, 4, 6, 7,   // 前平面
        8, 9, 10, 8, 10, 11,// 左平面
        12, 13, 14, 12, 14, 15,// 右平面
        16, 17, 18, 16, 18, 19,// 下平面
        20, 21, 22, 20, 22, 23 // 上平面
    };

    // 创建并返回Mesh对象
    return Mesh(cubeVertices, cubeIndices, textures);
}


#endif