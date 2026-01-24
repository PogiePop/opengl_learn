#ifndef _WA_MODEL_HPP
#define _WA_MODEL_HPP
#include <mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

class Model
{
public:
Model(const std::string& path){ Init(path); }
inline void Draw(Shader& sd);
private:
std::vector<Mesh>meshes;
std::string directory;
std::vector<Texture> loadedTextures;
private:
inline void Init(const std::string&);
inline void ProcessNode(const aiScene*, const aiNode*);
inline Mesh ProcessMesh(const aiScene*, const aiMesh*);
inline std::vector<Texture> GetTexturesFromMaterial(const aiMaterial*,const aiTextureType&, const std::string&);
};

inline std::string ComposeDirectoryAndPath(const std::string&, const std::string&);

inline void Model::Init(const std::string& path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::println("error::info::{}", import.GetErrorString());
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene, scene->mRootNode);
}


inline void Model::ProcessNode(const aiScene* scene, const aiNode* node)
{
    //处理根节点
    for(int i = 0; i < (int)node->mNumMeshes; i++)
        meshes.push_back(ProcessMesh(scene, scene->mMeshes[node->mMeshes[i]]));
    
    //处理子节点
    for(int i = 0; i < (int)node->mNumChildren; i++)
        ProcessNode(scene, node->mChildren[i]);
}


inline Mesh Model::ProcessMesh(const aiScene* scene, const aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(int i = 0; i < (int)mesh->mNumVertices; i++)
    {
        Vertex vt;
        const aiVector3D& v = mesh->mVertices[i];
        const aiVector3D& nr = mesh->mNormals[i];
        vt.position = glm::vec3(v.x, v.y, v.z);
        vt.normal = glm::vec3(nr.x, nr.y, nr.z);
        
        if(mesh->mTextureCoords[0])
        {
            const aiVector3D& tc = mesh->mTextureCoords[0][i];
            vt.texCoord = glm::vec2(tc.x, tc.y);
            vertices.push_back(vt);
        }
    }

    for(int i = 0; i < (int)mesh->mNumFaces; i++)
    {
        const aiFace& fc = mesh->mFaces[i];
        for(int j = 0; j < (int)fc.mNumIndices; j++)
            indices.push_back(fc.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0)
    {
        const aiMaterial* mt = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMap = GetTexturesFromMaterial(mt, aiTextureType_DIFFUSE, "diffuse");
        std::vector<Texture> specularMap = GetTexturesFromMaterial(mt, aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
        textures.insert(textures.end(), specularMap.begin(), specularMap.end());
    }
    return Mesh(vertices, indices, textures);
}


inline std::vector<Texture> Model::GetTexturesFromMaterial(const aiMaterial* mt, const aiTextureType& _type, const std::string& name)
{
    std::vector<Texture> textures;
    for(int i = 0; i < (int)mt->GetTextureCount(_type); i++)
    {
        aiString str;
        bool load = true;
        mt->GetTexture(_type, i, &str);
        for(int j = 0; j < (int)loadedTextures.size(); j++)
            if(strcmp(str.C_Str(), loadedTextures[j].path.c_str()) == 0)
            {
                textures.push_back(loadedTextures[j]);
                load = false;
                break;
            }
        if(load)
        {
            std::string absolutePath = ComposeDirectoryAndPath(directory, str.C_Str());
            std::println("{},", absolutePath);
            Texture tex(absolutePath.c_str(), name.c_str());
            tex.path = str.C_Str();
            textures.push_back(tex);
            loadedTextures.push_back(tex);
        }
    }
    return textures;
}

inline std::string ComposeDirectoryAndPath(const std::string& directory, const std::string& path)
{
    if(directory.empty())return path;
    char back = directory.back();
    if(back != '/' && back != '\\')return directory + '/' + path;
    return directory + path;
}

inline void Model::Draw(Shader& sd)
{
    for(int i = 0; i < (int)meshes.size(); i++)
        meshes[i].Draw(sd);
}

#endif