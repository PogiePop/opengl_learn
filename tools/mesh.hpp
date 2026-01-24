#ifndef _WA_MESH_HPP
#define _WA_MESH_HPP
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <texture.hpp>
#include <shader.hpp>
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
    Mesh(const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices, 
        const std::vector<Texture> &textures): 
    vertices(vertices), indices(indices), textures(textures){ Init(vertices, indices); }
    inline void Draw(Shader &);
    void Bind(){ glBindVertexArray(vao); }    
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int vao, vbo, ebo;
private:
    inline void Init(const std::vector<Vertex> &, const std::vector<unsigned int> &);
};

inline void Mesh::Init(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (int)vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
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
    glDrawElements(GL_TRIANGLES, (int)indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

#endif