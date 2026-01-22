#ifndef _WA_LIGHT_HPP
#define _WA_LIGHT_HPP
#include <glm/glm.hpp>
#include <shader.hpp>
#include <string>
//定义灯光结构体

struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;

    void SetUniform(const std::string&, Shader&);
};

void PointLight::SetUniform(const std::string& name, Shader& sd)
{
    sd.SetVec3(name + ".position", position);
    sd.SetVec3(name + ".ambient", ambient);
    sd.SetVec3(name + ".diffuse", diffuse);
    sd.SetVec3(name + ".specular", specular);
    sd.SetFloat1(name + ".constant", constant);
    sd.SetFloat1(name + ".linear", linear);
    sd.SetFloat1(name + ".quadratic", quadratic);
}


struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float innerCutOff;
    float outerCutOff;
    void SetUniform(const std::string&, Shader&);
};


void SpotLight::SetUniform(const std::string& name, Shader& sd)
{
    sd.SetVec3(name + ".position", position);
    sd.SetVec3(name + ".direction", direction);
    sd.SetVec3(name + ".ambient", ambient);
    sd.SetVec3(name + ".diffuse", diffuse);
    sd.SetVec3(name + ".specular", specular);
    sd.SetFloat1(name + ".constant", constant);
    sd.SetFloat1(name + ".linear", linear);
    sd.SetFloat1(name + ".quadratic", quadratic);
    sd.SetFloat1(name + ".innerCutOff", innerCutOff);
    sd.SetFloat1(name + ".outerCutOff", outerCutOff);
}

struct ParallelLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    void SetUniform(const std::string&, Shader&);
};

void ParallelLight::SetUniform(const std::string& name, Shader& sd)
{
    sd.SetVec3(name + ".direction", direction);
    sd.SetVec3(name + ".ambient", ambient);
    sd.SetVec3(name + ".diffuse", diffuse);
    sd.SetVec3(name + ".specular", specular);
}




#endif