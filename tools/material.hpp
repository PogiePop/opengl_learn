#ifndef _WA_MATERIAL_HPP
#define _WA_MATERIAL_HPP
#include <shader.hpp>
#include <string>

struct Material
{
    int diffuse;
    int specular;
    float shininess;
    inline void SetUniform(const std::string&, Shader&);
};

inline void Material::SetUniform(const std::string& name, Shader& sd)
{
    sd.SetInt1(name + ".diffuse", diffuse);
    sd.SetInt1(name + ".specular", specular);
    sd.SetFloat1(name + ".shininess", shininess);
}

#endif