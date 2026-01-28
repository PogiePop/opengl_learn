#ifndef _WA_SHADER_HPP
#define _WA_SHADER_HPP
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
public:
    Shader() = default;
    Shader(const char *vertexShader, const char *geometryShader = nullptr, const char *fragmentShader = nullptr);
    void use() { glUseProgram(ID); }
    inline void SetMat4(const std::string &, const glm::mat4 &);
    inline void SetVec3(const std::string &, const glm::vec3 &);
    inline void SetFloat1(const std::string &, const float &);
    inline void SetInt1(const std::string &, const int &);
    inline void BindBlock(const std::string&, const int&);
private:
    unsigned int ID;
};

Shader::Shader(const char *vertexShader, const char *geometryShader, const char *fragmentShader)
{
    std::string vtx_str;
    std::string geom_str;
    std::string frag_str;
    std::ifstream vtx_ifsm;
    std::ifstream geom_ifsm;
    std::ifstream frag_ifsm;
    vtx_ifsm.exceptions(std::ios::badbit | std::ios::failbit);
    if (geometryShader)
        geom_ifsm.exceptions(std::ios::badbit | std::ios::failbit);
    if (fragmentShader)
        frag_ifsm.exceptions(std::ios::badbit | std::ios::failbit);
    try
    {
        vtx_ifsm.open(vertexShader);
        std::stringstream vtx_sstm;
        vtx_sstm << vtx_ifsm.rdbuf();
        vtx_str = vtx_sstm.str();
        if (geometryShader)
        {
            geom_ifsm.open(geometryShader);
            std::stringstream geom_sstm;
            geom_sstm << geom_ifsm.rdbuf();
            geom_str = geom_sstm.str();
        }
        if (fragmentShader)
        {
            frag_ifsm.open(fragmentShader);
            std::stringstream frag_sstm;
            frag_sstm << frag_ifsm.rdbuf();
            frag_str = frag_sstm.str();
        }
    }
    catch (std::ifstream::failure &e)
    {
        std::println("read shader file fail,{}", e.what());
        return;
    }
    // 创建和绑定shader对象
    unsigned int v_sd = 0, g_sd = 0, f_sd = 0;
    v_sd = glCreateShader(GL_VERTEX_SHADER);
    const char *v_cstr = vtx_str.c_str();
    glShaderSource(v_sd, 1, &v_cstr, nullptr);
    if (geometryShader)
    {
        g_sd = glCreateShader(GL_GEOMETRY_SHADER);
        const char *g_cstr = geom_str.c_str();
        glShaderSource(g_sd, 1, &g_cstr, nullptr);
    }
    if (fragmentShader)
    {
        f_sd = glCreateShader(GL_FRAGMENT_SHADER);
        const char *f_cstr = frag_str.c_str();
        glShaderSource(f_sd, 1, &f_cstr, nullptr);
    }
    int success = 0;
    char infoLog[512] = {NULL};
    glCompileShader(v_sd);
    glGetShaderiv(v_sd, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v_sd, 512, nullptr, infoLog);
        std::println("vertex shader compile fail,{}", infoLog);
        return;
    }
    if (geometryShader)
    {
        glCompileShader(g_sd);
        glGetShaderiv(g_sd, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(g_sd, 512, nullptr, infoLog);
            std::println("geometry shader compile fail,{}", infoLog);
            return;
        }
    }
    if (fragmentShader)
    {
        glCompileShader(f_sd);
        glGetShaderiv(f_sd, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(f_sd, 512, nullptr, infoLog);
            std::println("fragment shader compile fail,{}", infoLog);
            return;
        }
    }
    // 创建着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, v_sd);
    if (geometryShader)
        glAttachShader(ID, g_sd);
    if (fragmentShader)
        glAttachShader(ID, f_sd);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::println("shader program link fail,{}", infoLog);
        return;
    }
    glDeleteShader(v_sd);
    if (geometryShader)
        glDeleteShader(g_sd);
    if (fragmentShader)
        glDeleteShader(f_sd);
}

inline void Shader::SetMat4(const std::string &name, const glm::mat4 &value)
{
    int location = glGetUniformLocation(ID, name.c_str());
    if(location != -1)glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
inline void Shader::SetVec3(const std::string &name, const glm::vec3 &value)
{
    int location = glGetUniformLocation(ID, name.c_str());
    if(location != -1)glUniform3fv(location, 1, glm::value_ptr(value));
}
inline void Shader::SetFloat1(const std::string &name, const float &value)
{
    int location = glGetUniformLocation(ID, name.c_str());
    if(location != -1)glUniform1f(location, value);
}
inline void Shader::SetInt1(const std::string &name, const int &value)
{
    int location = glGetUniformLocation(ID, name.c_str());
    if(location != -1)glUniform1i(location, value);
}

inline void Shader::BindBlock(const std::string& name, const int& bknum)
{
    int location = glGetUniformBlockIndex(ID, name.c_str());
    if(location != -1)glUniformBlockBinding(ID, location, bknum);
}

#endif