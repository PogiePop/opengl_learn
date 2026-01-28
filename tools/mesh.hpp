#ifndef _WA_MESH_HPP
#define _WA_MESH_HPP
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <texture.hpp>
#include <shader.hpp>

const std::vector<Texture> _empty_tex = std::vector<Texture>();
const std::vector<unsigned int> _empty_ids = std::vector<unsigned int>();
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    Vertex(){}
    Vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal): 
    position(position), texCoord(texCoord), normal(normal){}
};

class Mesh
{
public:
    Mesh() = default;  // 核心添加
    Mesh(const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices, 
        const std::vector<Texture> &textures,
        bool isSubMod = false): 
    vertices(vertices), indices(indices), textures(textures){
        if(!isSubMod)
            Init(vertices, indices);
        else
            InitSub(vertices, indices);
        }
    inline void Draw(Shader &);
    inline void DrawCubeMap(Shader &);
    inline void DrawPoint(float);
    void Bind(){ glBindVertexArray(vao); }
    void AddTexture(const std::vector<Texture>& textures){ this->textures.insert(this->textures.end(), textures.begin(), textures.end()); }    
    void AddTexture(const Texture& tex){ this->textures.push_back(tex); }
    void ClearTextures(){ this->textures.clear(); }
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int vao, vbo, ebo;
private:
    inline void Init(const std::vector<Vertex> &, const std::vector<unsigned int> &);
    inline void InitSub(const std::vector<Vertex>&, const std::vector<unsigned int>&);
};

inline void Mesh::Init(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (int)vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    if(!indices.empty()){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
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

inline void Mesh::Draw(Shader &sd)
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

#endif