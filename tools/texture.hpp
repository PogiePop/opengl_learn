#ifndef _WA_TEXTURE_HPP
#define _WA_TEXTURE_HPP
#include <glad/glad.h>
#include "stb_image.h"
#include <string>
#include <print>
#include <vector>
#include <Windows.h>

class Texture
{
public:
    Texture() = default;
    Texture(const char *path, const char *type = nullptr, bool isFlipUV = false);
    Texture(unsigned int ID, const char *type)
    {
        this->ID = ID;
        this->type = type;
    }
    Texture(const std::vector<std::string> &faces) { LoadCubeMap(faces); }
    void Bind() { glBindTexture(GL_TEXTURE_2D, ID); }
    void BindCubeMap() { glBindTexture(GL_TEXTURE_CUBE_MAP, ID); }
    unsigned int GetID() { return ID; }

public:
    std::string path;
    std::string type;

private:
    unsigned int ID = 0;

private:
    inline void Init(const char *path, bool isFlipUV = false);
    inline void LoadCubeMap(const std::vector<std::string> &);
};

Texture::Texture(const char *path, const char *type, bool isFlipUV)
{
    if (path)
    {
        this->path = path;
        Init(path, isFlipUV);
    }
    if (type)
        this->type = type;
}

// 将读取路径转化为宽字符
std::wstring utf8ToWstring(const std::string &utf8Str)
{
    if (utf8Str.empty())
        return L"";
    // 获取所需缓冲区大小
    int wcharLen = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8Str.c_str(),
        -1,
        nullptr,
        0);
    if (wcharLen == 0)
    {
        throw std::runtime_error("UTF8转宽字符失败！错误码：" + std::to_string(GetLastError()));
    }

    std::wstring wstr(wcharLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], wcharLen);
    // 移除末尾的'\0'
    wstr.pop_back();
    return wstr;
}

// 将内容读取到内存
std::vector<unsigned char> readFileToMemory(const std::wstring &wpath)
{
    HANDLE hFile = CreateFileW(
        wpath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        DWORD errorCode = GetLastError();
        std::string errorMsg = "无法打开文件" + std::string(wpath.begin(), wpath.end()) + ", 错误码:" + std::to_string(errorCode);
        throw std::runtime_error(errorMsg);
    }

    // 获取文件大小
    DWORD fileSize = GetFileSize(hFile, nullptr);
    if (fileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        DWORD errorCode = GetLastError();
        throw std::runtime_error("获取文件大小失败，错误码：" + std::to_string(errorCode));
    }

    // 分配内存缓冲区并读取文件
    std::vector<unsigned char> buffer(fileSize);
    DWORD bytesRead = 0;
    BOOL readSuccess = ReadFile(
        hFile,
        buffer.data(),
        fileSize,
        &bytesRead,
        nullptr);

    CloseHandle(hFile);

    // 检查是否读取成功
    if (!readSuccess || bytesRead != fileSize)
    {
        DWORD errorCode = GetLastError();
        throw std::runtime_error("读取文件失败，错误码：" + std::to_string(errorCode));
    }
    return buffer;
}

inline void Texture::Init(const char *path, bool isFlipUV)
{
    stbi_set_flip_vertically_on_load(isFlipUV);
    int width, height, nrChannel;
    // 读取文件
    try
    {
        std::wstring wpath = utf8ToWstring(std::string(path));
        std::vector<unsigned char> buffer = readFileToMemory(wpath);
        unsigned char *data = stbi_load_from_memory(buffer.data(), (int)buffer.size(), &width, &height, &nrChannel, 0);
        if (!data)
        {
            std::println("加载纹理失败");
            return;
        }
        GLenum format, internalformat;
        if (nrChannel == 1)
        {
            format = GL_RED;
            internalformat = GL_RED;
        }
        else if (nrChannel == 3)
        {
            format = GL_RGB;
            internalformat = GL_RGB;
        }
        else if (nrChannel == 4)
        {
            format = GL_RGBA;
            internalformat = GL_RGBA;
        }
        else
        {
            std::println("没有此类型");
            stbi_image_free(data);
            return;
        }
        // 创建纹理对象
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 设置环绕格式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    catch (std::exception &e)
    {
        std::println("{}", e.what());
    }
}

inline void Texture::LoadCubeMap(const std::vector<std::string> &faces)
{
    int width, height, nrChannel;
    // 创建纹理对象
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    for (int i = 0; i < (int)faces.size(); i++)
    {
        try
        {
            std::wstring wpath = utf8ToWstring(faces[i]);
            std::vector<byte> buffer = readFileToMemory(wpath);
            byte *data = stbi_load_from_memory(buffer.data(), (int)buffer.size(), &width, &height, &nrChannel, 0);
            if (!data)
            {
                std::println("加载纹理失败");
                return;
            }
            GLenum format, internalformat;
            if (nrChannel == 1)
            {
                format = GL_RED;
                internalformat = GL_RED;
            }
            else if (nrChannel == 3)
            {
                format = GL_RGB;
                internalformat = GL_RGB;
            }
            else if (nrChannel == 4)
            {
                format = GL_RGBA;
                internalformat = GL_RGBA;
            }
            else
            {
                std::println("没有此类型");
                stbi_image_free(data);
                return;
            }
            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            // 设置环绕格式
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
            type = "diffuse"; 
        }
        catch (const std::exception &e)
        {
            glDeleteTextures(1, &ID);
            std::println("立方体贴图错误：{}", e.what());
        }
    }
}

#endif