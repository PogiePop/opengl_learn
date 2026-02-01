#ifndef _WA_MESH_UTILS_HPP
#define _WA_MESH_UTILS_HPP
#include <mesh.hpp>



Mesh CreateCubeMesh(float, const std::vector<Texture> &, bool);
Mesh CreateQuadMesh(float, const std::vector<Texture> &, bool);
Mesh CreateQuadMeshInstance(float, const std::vector<Texture> &, const std::vector<glm::vec3>&,
                    const std::vector<glm::vec3>&, bool);
Mesh CreatePointMesh(const std::vector<glm::vec3>& points, 
                     float pointSize = 5.0f,  // 点的大小（渲染时用glPointSize设置）
                     const std::vector<Texture>& textures = _empty);

Mesh CreateCubeMesh(float size = 1.0f, const std::vector<Texture> &textures = _empty, bool isSubMod = false)
{
    // 计算半长（立方体中心在原点，各轴范围 [-halfSize, halfSize]）
    float halfSize = size / 2.0f;

    // 立方体顶点数据（24个顶点：6个面 × 4个顶点）
    // 核心修正：所有面的顶点顺序统一为「从外部看逆时针」
    std::vector<Vertex> cubeVertices = {
        // 后平面 (z = -halfSize) - 法线：(0,0,-1)
        // 顶点顺序：左下 → 右下 → 右上 → 左上（外部看逆时针）
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},

        // 前平面 (z = halfSize) - 法线：(0,0,1)
        // 顶点顺序：右下 → 左下 → 左上 → 右上（外部看逆时针）
        {{halfSize, -halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, -halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, halfSize, halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, halfSize, halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},

        // 左平面 (x = -halfSize) - 法线：(-1,0,0)
        // 顶点顺序：右上 → 右下 → 左下 → 左上（外部看逆时针）
        {{-halfSize, halfSize, halfSize}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, -halfSize, halfSize}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, halfSize, -halfSize}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},

        // 右平面 (x = halfSize) - 法线：(1,0,0)
        // 顶点顺序：左上 → 左下 → 右下 → 右上（外部看逆时针）
        {{halfSize, halfSize, -halfSize}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, -halfSize, halfSize}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, halfSize, halfSize}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},

        // 下平面 (y = -halfSize) - 法线：(0,-1,0)
        // 顶点顺序：左上 → 右上 → 右下 → 左下（外部看逆时针）
        {{-halfSize, -halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, -halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, -halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, -halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},

        // 上平面 (y = halfSize) - 法线：(0,1,0)
        // 顶点顺序：左下 → 右下 → 右上 → 左上（外部看逆时针）
        {{-halfSize, halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{halfSize, halfSize, halfSize}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{-halfSize, halfSize, halfSize}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}};

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
    return Mesh(cubeVertices, cubeIndices, textures, isSubMod);
}

Mesh CreateQuadMesh(float size = 1.0f, const std::vector<Texture> &textures = _empty, bool isSubMod = false)
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
    return Mesh(quadVertices, quadIndices, textures, isSubMod);
}

Mesh CreateQuadMeshInstance(float size = 1.0f, const std::vector<Texture> &textures = _empty, 
const std::vector<glm::vec3>& color = _vec3_ep, const std::vector<glm::vec3>& offset = _vec3_ep, 
bool isSubMod = false)
{
    // 半长：四边形沿X/Y轴的半尺寸，中心在(0,0,0)，平面Z=0
    float halfSize = size / 2.0f;

    // 4个顶点：左下、右下、右上、左上（符合右手坐标系，纹理坐标常规映射）
    // 顶点结构：position(glm::vec3) + texCoord(glm::vec2) + normal(glm::vec3)
    std::vector<Vertex> quadVertices = {
        // 左下 (X-, Y-, Z0) | 纹理(0,0) | 法线+Z
        {{-halfSize, -halfSize, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, color[0]},
        // 右下 (X+, Y-, Z0) | 纹理(1,0) | 法线+Z
        {{halfSize, -halfSize, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, color[1]},
        // 右上 (X+, Y+, Z0) | 纹理(1,1) | 法线+Z
        {{halfSize, halfSize, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, color[2]},
        // 左上 (X-, Y+, Z0) | 纹理(0,1) | 法线+Z
        {{-halfSize, halfSize, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, color[3]}};
    // 6个索引：2个三角形拼接四边形（0-1-2 + 0-2-3），无顶点复用
    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        0, 2, 3};
    std::vector<InstanceData> instances;
    if(!offset.empty())
    {
        for(const auto& elem : offset)
            instances.push_back({elem});
    }

    // 直接返回Mesh对象，复用现有Mesh类的VAO/VBO/EBO初始化逻辑
    return Mesh(quadVertices, quadIndices, textures, instances,isSubMod);
}


Mesh CreatePointMesh(const std::vector<glm::vec3>& points, 
                     float pointSize, 
                     const std::vector<Texture>& textures) {
    // 转换点位置为Vertex结构体（纹理坐标/法线设为默认值，点渲染无需这些）
    std::vector<Vertex> pointVertices;
    for (const auto& pos : points) {
        // Vertex参数：position(点位置) + texCoord(默认0,0) + normal(默认0,0,0)
        pointVertices.emplace_back(pos, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // 点渲染无需索引（EBO），传空向量
    std::vector<unsigned int> emptyIndices;

    // 创建Mesh对象（Init方法兼容空索引，仅创建VAO/VBO，不创建EBO）
    Mesh pointMesh(pointVertices, emptyIndices, textures);
    
    // 注意：pointSize只是参数存储，实际渲染时需调用glPointSize(pointSize)
    return pointMesh;
}

Mesh CreatePointMesh(const glm::vec3& singlePoint, 
                     float pointSize = 5.0f, 
                     const std::vector<Texture>& textures = _empty) {
    return CreatePointMesh(std::vector<glm::vec3>{singlePoint}, pointSize, textures);
}

#endif