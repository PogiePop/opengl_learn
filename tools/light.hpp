#ifndef _WA_LIGHT_HPP
#define _WA_LIGHT_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.hpp>
#include <string>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#ifndef UP
#define UP glm::vec3(0.0f, 1.0f, 0.0f)
#endif

#ifndef FRONT
#define FRONT glm::vec3(0.0f, 0.0f, 1.0f)
#endif

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

    inline void SetUniform(const std::string&, Shader&);
    inline glm::mat4 GetProjectionMatrix(float aspect = 1.0f, float near = 1.0f, float far = 25.0f)const;
    inline  std::vector<glm::mat4> GetViewMatrices() const;
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

glm::mat4 PointLight::GetProjectionMatrix(float aspect, float near, float far)const {
    return glm::perspective(glm::radians(90.0f), aspect, near, far);
}

inline std::vector<glm::mat4> PointLight::GetViewMatrices() const {
    std::vector<glm::mat4> matrices;
    //按照右左上下前后
    matrices.push_back(glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f)
        , UP));
    matrices.push_back(glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f)
        , UP));
    matrices.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f)
        , FRONT));
    matrices.push_back(glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f)
        , FRONT));
    matrices.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f)
       , UP));
    matrices.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f)
        , UP));
    return matrices;
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
    inline void SetUniform(const std::string&, Shader&);
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

const static glm::vec3 _vt_pos = glm::vec3(-2.0f, 4.0f, -1.0f);

struct ParallelLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    inline void SetUniform(const std::string&, Shader&);
    inline  glm::mat4 GetLookAtMatrix(const glm::vec3& virtualPos = _vt_pos);
    inline  glm::mat4 GetProjectionMatrix(float size = 10.0f, float near_pl = 1.0f, float far_pl = 7.5f);
};

inline  void ParallelLight::SetUniform(const std::string& name, Shader& sd)
{
    sd.SetVec3(name + ".direction", direction);
    sd.SetVec3(name + ".ambient", ambient);
    sd.SetVec3(name + ".diffuse", diffuse);
    sd.SetVec3(name + ".specular", specular);
}

glm::mat4 ParallelLight::GetLookAtMatrix(const glm::vec3& virtualPos)
{
    return glm::lookAt(
        virtualPos,
        virtualPos + direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::mat4 ParallelLight::GetProjectionMatrix(float size, float near_pl, float far_pl)
{
    return glm::ortho(-size, size, -size, size, near_pl, far_pl);
}




#endif