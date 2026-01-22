#include <print>
#include <window.hpp>
#include <shader.hpp>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <texture.hpp>
#include <camera.hpp>
#define RES_PATH "../../res/"

// 立方体顶点数组：每个顶点 = 3个位置分量 + 2个纹理坐标分量
float cubeVertices[] = {
    // 前侧面 (z=0.5)
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // 左上
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 右上
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 左下
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // 右下

    // 后侧面 (z=-0.5)
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 左上（注意纹理坐标翻转，避免镜像）
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 右上
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // 左下
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 右下

    // 左侧面 (x=-0.5)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 左上
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 右上
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 左下
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // 右下

    // 右侧面 (x=0.5)
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // 左上
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 右上
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 左下
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // 右下

    // 上侧面 (y=0.5)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 左上
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 右上
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // 左下
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // 右下

    // 下侧面 (y=-0.5)
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // 左上
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // 右上
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 左下
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f   // 右下
};

// 立方体索引数组：每个面2个三角形，共6面×6索引=36个索引
unsigned int cubeIndices[] = {
    // 前侧
    0, 1, 2,  2, 3, 1,
    // 后侧
    4, 5, 6,  6, 7, 5,
    // 左侧
    8, 9,10, 10,11, 9,
    // 右侧
   12,13,14, 14,15,13,
    // 上侧
   16,17,18, 18,19,17,
    // 下侧
   20,21,22, 22,23,21
};

int main()
{
    Window _window;
    Camera camera;
    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        //std::println("{}X{}", xOffset, yOffset);
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));
   
    unsigned int vao, vbo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    Shader tri(RES_PATH "shaders/tri.vert", nullptr, RES_PATH "shaders/tri.frag");
   
    Texture t1(RES_PATH "textures/wo1.png", nullptr);
    Texture t2(RES_PATH "textures/l1.png", nullptr);
    
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        tri.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth()/_window.GetHeight(), 0.1f, 100.0f);
        tri.SetMat4("model", model);
        tri.SetMat4("view", view);
        tri.SetMat4("projection", projection);
        tri.SetInt1("texture0", 0);
        tri.SetInt1("texture1", 1);
        glActiveTexture(GL_TEXTURE0);
        t1.Bind();
        glActiveTexture(GL_TEXTURE1);
        t2.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
    });
    return 0;
}