#ifndef _WA_MESH_HPP
#define _WA_MESH_HPP
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <texture.hpp>
#include <shader.hpp>

const std::vector<Texture> _empty_tex = std::vector<Texture>();
const std::vector<unsigned int> _empty_ids = std::vector<unsigned int>();
const static std::vector<Texture> _empty = std::vector<Texture>();
const static std::vector<glm::vec3> _vec3_ep = std::vector<glm::vec3>();
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 color;//没有纹理时使用

    glm::vec3 tangent;//主切线
    glm::vec3 bitangent;//副切线
    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal): 
    position(position), texCoord(texCoord), normal(normal){}
    Vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal, glm::vec3 color): 
    position(position), texCoord(texCoord), normal(normal), color(color){}
    Vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal, glm::vec3 color, glm::vec3 tangent, glm::vec3 bitangent):
    position(position), texCoord(texCoord), normal(normal), color(color), tangent(tangent), bitangent(bitangent) {}
};

struct InstanceData
{
    glm::vec3 offset; //偏移
};
const static std::vector<InstanceData> _instance_ep =std::vector<InstanceData>();

class Mesh
{
public:
    Mesh() = default;  // 核心添加
    Mesh(const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices, 
        const std::vector<Texture> &textures,
        bool isSubMod = false,
        bool isNormalPt = false,
        bool hasNormalPt = false
        ):
    vertices(vertices), indices(indices), textures(textures), hasNormalPt(hasNormalPt){
        if(!isSubMod)
            Init(vertices, indices, _instance_ep, isNormalPt);
        else
            InitSub(vertices, indices);
        }
    Mesh(const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices, 
        const std::vector<Texture> &textures,
        const std::vector<InstanceData>& instances,
        bool isSubMod = false):
    vertices(vertices), indices(indices), textures(textures), instances(instances){
        if(!isSubMod)
            Init(vertices, indices, instances);
        else
            InitSub(vertices, indices);
        }

    // Mesh(const std::vector<Vertex> &vertices,
    //     const std::vector<unsigned int> &indices,
    //     const std::vector<Texture> &textures,
    //     bool isNormalPt,
    //     bool isSubMod = false):
    // vertices(vertices), indices(indices), textures(textures){
    //     if(!isSubMod)
    //         Init(vertices, indices, _instance_ep, isNormalPt);
    //     else
    //         InitSub(vertices, indices);
    // }
    inline void Draw(const Shader &)const;
    inline void DrawCubeMap(Shader &);
    inline void DrawPoint(float);
    inline void DrawInstance(Shader&, int);
    void Bind()const{ glBindVertexArray(vao); }
    void AddTexture(const std::vector<Texture>& textures){ this->textures.insert(this->textures.end(), textures.begin(), textures.end()); }    
    void AddTexture(const Texture& tex){ this->textures.push_back(tex); }
    void ModifyColor(const std::vector<glm::vec3>&);
    void ModifyOffset(const std::vector<glm::vec3>&);
    void ClearTextures(){ this->textures.clear(); }
    int GetInstanceNum(){ return (int)this->instances.size(); }
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::vector<InstanceData> instances; //实例化数据池
    unsigned int vao, vbo, ebo, instanceVbo = 0;
    bool hasNormalPt = false;
private:
    inline void Init(const std::vector<Vertex> &, const std::vector<unsigned int> &, const std::vector<InstanceData>&instances = _instance_ep, bool isNormalPt = false);
    inline void InitSub(const std::vector<Vertex>&, const std::vector<unsigned int>&);
    inline void ComputeTBN();
};

inline void Mesh::Init(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<InstanceData>&instances, bool isNormalPt)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (isNormalPt && !hasNormalPt) ComputeTBN();
    glBufferData(GL_ARRAY_BUFFER, (int)this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
    if(!indices.empty()){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);
    }
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    //添加color和offset
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
    if(!instances.empty())
    {
        glGenBuffers(1, &instanceVbo);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * instances.size(), instances.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
        glVertexAttribDivisor(4, 1);
    }

    if (isNormalPt) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3,GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    }
    glBindVertexArray(0);
}

inline void Mesh::InitSub(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
    std::vector<glm::vec3> position;
    std::vector<glm::vec2> texCoord;
    std::vector<glm::vec3> normal;
    for(const auto& elem : vertices)
    {
        position.push_back(elem.position);
        texCoord.push_back(elem.texCoord);
        normal.push_back(elem.normal);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, position.size() * sizeof(glm::vec3), position.data());
    glBufferSubData(GL_ARRAY_BUFFER, position.size() * sizeof(glm::vec3), texCoord.size() * sizeof(glm::vec2), texCoord.data());
    glBufferSubData(GL_ARRAY_BUFFER, position.size() * sizeof(glm::vec3) + texCoord.size() * sizeof(glm::vec2), normal.size() * sizeof(glm::vec3), normal.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(position.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(position.size() * sizeof(glm::vec3) + texCoord.size() * sizeof(glm::vec2)));
    glBindVertexArray(0);
}

inline void Mesh::Draw(const Shader &sd)const
{
    int diffNum = 0, specNum = 0, normal = 0, depth = 0, other = 0;
    for(int i = 0; i < (int)textures.size(); i++)
    {
        std::string name = "";
        if(textures[i].type == "diffuse")
        {
            name += "material.diffuse" + std::to_string(diffNum++);
        }
        else if(textures[i].type == "specular")
        {
            name += "material.specular" + std::to_string(specNum++);
        }
        else if (textures[i].type == "normal") {
            name += "material.normal" + std::to_string(normal++);
        }
        else if (textures[i].type == "depth") {
            name += "material.depth" + std::to_string(depth++);
        }
        else
        {
            name += "material.other" + std::to_string(other++);
        }
        //std::println("texture name:{},", name);
        glActiveTexture(GL_TEXTURE0 + i);
        textures[i].Bind();
        sd.SetInt1(name, i);
    }
    glActiveTexture(GL_TEXTURE0);
    Bind();
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

inline void Mesh::DrawCubeMap(Shader & sd)
{
    for(int i = 0; i < (int)textures.size(); i++)
    {
        std::string name = "material.diffuse";
        glActiveTexture(GL_TEXTURE0 + i);
        textures[i].BindCubeMap();
        sd.SetInt1(name + std::to_string(i), i);
    }
    glActiveTexture(GL_TEXTURE0);
    Bind();
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

inline void Mesh::DrawPoint(float size = 1.0f)
{
    Bind();
    glPointSize(size);
    glDrawArrays(GL_POINTS, 0, (int)vertices.size());
}

inline void Mesh::DrawInstance(Shader& sd, int count = 10)
{
    int diffNum = 0, specNum = 0, other = 0;
    for(int i = 0; i < (int)textures.size(); i++)
    {
        std::string name = "";
        if(textures[i].type == "diffuse")
        {
            name += "material.diffuse" + std::to_string(diffNum++);
        }
        else if(textures[i].type == "specular")
        {
            name += "material.specular" + std::to_string(specNum++);
        }
        else
        {
            name += "material.other" + std::to_string(other++);
        }
        //std::println("texture name:{},", name);
        glActiveTexture(GL_TEXTURE0 + i);
        textures[i].Bind();
        sd.SetInt1(name, i);
    }
    glActiveTexture(GL_TEXTURE0);
    Bind();
    glDrawElementsInstanced(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, nullptr, count);
    glBindVertexArray(0);
}

void Mesh::ModifyColor(const std::vector<glm::vec3>& colors)
{
    if(colors.empty())return;
    for(int i = 0; i < (int)vertices.size() && (int)colors.size(); i++)
        vertices[i].color = colors[i];
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //更新缓冲
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Mesh::ModifyOffset(const std::vector<glm::vec3>& offsets)
{
    if(offsets.empty())return;
    instances.clear();
    for(const auto& elem : offsets)instances.push_back({elem});
    glBindVertexArray(vao);
    if(instanceVbo == 0)
    {
        glGenBuffers(1, &instanceVbo);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * instances.size(), instances.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
        glVertexAttribDivisor(4, 1);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * instances.size(), instances.data(), GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
}


inline void Mesh::ComputeTBN() {
    std::vector<glm::vec3> tangents(vertices.size(), glm::vec3(0.0f)), bitangents(vertices.size(), glm::vec3(0.0f));
    for (size_t i = 0; i < indices.size(); i+=3) {
        glm::vec3 tangent, bitangent;
        unsigned int ix1 = indices[i];
        unsigned int ix2 = indices[i+1];
        unsigned int ix3 = indices[i+2];
        glm::vec3 pos1 = vertices[ix1].position;
        glm::vec3 pos2 = vertices[ix2].position;
        glm::vec3 pos3 = vertices[ix3].position;
        glm::vec2 uv1 = vertices[ix1].texCoord;
        glm::vec2 uv2 = vertices[ix2].texCoord;
        glm::vec2 uv3 = vertices[ix3].texCoord;
        glm::vec2 deltaUV12 = uv2 - uv1;
        glm::vec2 deltaUV13 = uv3 - uv1;
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        float k = 1.0f / (deltaUV12.x * deltaUV13.y - deltaUV13.x * deltaUV12.y);
        tangent.x = k * (deltaUV13.y * edge1.x - deltaUV12.y * edge2.x);
        tangent.y = k * (deltaUV13.y * edge1.y - deltaUV12.y * edge2.y);
        tangent.z = k * (deltaUV13.y * edge1.z - deltaUV12.y * edge2.z);
        bitangent.x = k * (-deltaUV13.x * edge1.x + deltaUV12.x * edge2.x);
        bitangent.y = k * (-deltaUV13.x * edge1.y + deltaUV12.x * edge2.y);
        bitangent.z = k * (-deltaUV13.x * edge1.z + deltaUV12.x * edge2.z);
        tangents[ix1] += tangent;
        tangents[ix2] += tangent;
        tangents[ix3] += tangent;
        bitangents[ix1] += bitangent;
        bitangents[ix2] += bitangent;
        bitangents[ix3] += bitangent;
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        glm::vec3 T =glm::normalize(tangents[i] - vertices[i].normal * glm::dot(vertices[i].normal, tangents[i]));
        vertices[i].tangent = T;
        vertices[i].bitangent = glm::normalize(glm::cross(vertices[i].normal, T));
    }
}

#endif